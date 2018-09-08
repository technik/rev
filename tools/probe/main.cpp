//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math/algebra/vector.h>
#include <math/numericTraits.h>
#include <math/noise.h>
#include <nlohmann/json.hpp>

#include <core/platform/osHandler.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/driver/shader.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/Image.h>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// Convert latlong images into environment probes

using namespace std;
using namespace rev::math;
using namespace rev::gfx;
using Json = nlohmann::json;

struct Params {
	std::string in;
	std::string out;

#ifdef _WIN32
	static constexpr size_t arg0 = 1;
#else
	static constexpr size_t arg0 = 0;
#endif

	bool parseArguments(int _argc, const char** _argv) {
		for (int i = arg0; i < _argc; ++i) {
			const char* arg = _argv[i];
			if (0 == strcmp(arg, "-i")) { // Input file
				++i;
				if (i == _argc) {
					cout << "Error: missing input filename after -i\n";
					return false;
				}
				in = _argv[i];
			}
			else if (0 == strcmp(arg, "-o")) { // Output file
				++i;
				if (i == _argc) {
					cout << "Error: missing output filename after -o\n";
					return false;
				}
				out = _argv[i];
			}
		} // for
		return true;
	}
};

auto extension(const std::string_view& s)
{
	auto dot = s.find_last_of(".");
	if (dot == std::string_view::npos)
		return std::string_view("");
	else
		return s.substr(dot+1);
}

// clamp t into [a,b]
float clamp(float t, float a, float b)
{
	return max(a, min(b, t));
}

Vec3f latLong2Sphere(float u, float v)
{
	auto z = sin(Pi*(v - 0.5f));
	auto cosPhi = sqrt(1-z*z);
	auto theta = TwoPi*u-Pi;
	auto x = cos(theta) * cosPhi;
	auto y = sin(theta) * cosPhi;
	return {x,y,z};
}

Vec2f sphere2LatLong(const Vec3f& dir)
{
	auto u = atan2(dir.y(), dir.x()) / TwoPi + 0.5f;
	auto v = (float)(asin(-dir.z()) / Pi + 0.5f);
	return {u, v};
}

struct Image
{
	Image(int sx, int sy)
		: nx(sx), ny(sy)
	{
		m = new Vec3f[nx*ny];
	}

	static Image* constantImage(int sx, int sy, float c)
	{
		Image* m = new Image(sx, sy);
		Vec3f color = Vec3f(c,c,c);
		for(int i = 0; i < m->nPixels(); ++i)
		{
			m->at(i) = color;
		}

		return m;
	}

	~Image() { delete[] m; }

	int nPixels() const { return nx*ny; }

	Vec3f& at(int x, int y)
	{
		return m[x+y*nx];
	}

	Vec3f& at(int i)
	{
		return m[i];
	}

	const Vec3f& at(int x, int y) const
	{
		return m[x+y*nx];
	}

	const Vec3f& at(int i) const
	{
		return m[i];
	}

	void saveHDR(const std::string& fileName) const
	{
		if(extension(fileName) != "hdr")
		{
			cout << "Only .hdr is supported for hdr output images\n";
		}

		auto src = reinterpret_cast<const float*>(m);
		stbi_write_hdr(fileName.c_str(), nx, ny, 3, src);
	}

	void save2sRGB(const std::string& fileName) const
	{
		if(extension(fileName) != "png")
		{
			cout << "Only png is supported for output images\n";
		}
		auto nBytes = 3*nx*ny;
		std::vector<uint8_t> raw(nBytes);
		auto src = reinterpret_cast<const float*>(m);

		// Linear to sRGB conversion following the formal specification of sRGB
		for(int i = 0; i < nBytes; ++i)
		{
			auto linear = src[i];
			float srgb;
			if(linear <= 0.0031308f)
			{
				srgb = uint8_t(clamp(linear * 3294.6f, 0.f, 255.f)); 
			}
			else
			{
				srgb = 269.025f * pow(linear,1/2.4f) - 0.055f;
			}
			raw[i] = (uint8_t)clamp(srgb, 0.f, 255.f);
		}
		auto bytesPerRow = 3*nx;
		stbi_write_png(fileName.c_str(), nx, ny, 3, raw.data(), bytesPerRow);
	}

	void saveLinear(const std::string& fileName) const
	{
		if(extension(fileName) != "png")
		{
			cout << "Only png is supported for output images\n";
		}
		auto nBytes = 3*nx*ny;
		std::vector<uint8_t> raw(nBytes);
		auto src = reinterpret_cast<const float*>(m);

		// Linear to sRGB conversion following the formal specification of sRGB
		for(int i = 0; i < nBytes; ++i)
		{
			auto linear = src[i] * 255.f;
			raw[i] = (uint8_t)clamp(linear, 0.f, 255.f);
		}
		auto bytesPerRow = 3*nx;
		stbi_write_png(fileName.c_str(), nx, ny, 3, raw.data(), bytesPerRow);
	}

	Image* reduce2x() const
	{
		if(nx & 1 || ny & 1) // Odd sizes not supported
			return nullptr;

		auto reduced = new Image(nx/2, ny/2);
		for(int i = 0; i < ny/2; ++i)
		{
			auto y = 2*i;
			for(int j = 0; j < nx/2; ++j)
			{
				auto x = 2*j;
				reduced->at(j,i) = 0.25f * (at(x,y)+at(x+1,y)+at(x,y+1)+at(x+1,y+1));
			}
		}

		return reduced;
	}

	std::vector<Image*> generateMipMaps()
	{
		std::vector<Image*> mips;
		auto mip = this;
		while(mip && mip->nx > 16 && mip->ny > 16)
		{
			mips.push_back(mip);
			mip = mip->reduce2x();
		}
		return mips;
	}

	Vec3f convolveRadiance(RandomGenerator& random, size_t nSamples, const Vec3f& normal) const
	{
		Vec3f accum = Vec3f(0.f, 0.f, 0.f);
		for(size_t n = 0; n < nSamples;)
		{
			// Sample on the hemisphere of incomming radiance
			auto sampleDir = random.unit_vector();
			auto ndv = dot(sampleDir, normal);
			if(ndv > 0)
			{
				++n;
				accum = accum + sampleSpherical(sampleDir) * ndv;
			}
		}
		return accum * (TwoPi/nSamples);
	}

	float RadicalInverse_VdC(size_t bits) 
	{
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
	}

	// Precompute random samples
	void pregenerateRandomSamples(size_t nSamples)
	{
		randomSamples.resize(nSamples);
		const float deltaX0 = 1.f/nSamples;
		for(size_t i = 0; i < nSamples; ++i)
		{
			randomSamples[i] = Vec2f(i*deltaX0, RadicalInverse_VdC(i));
		}
	}

	// Precompute random samples
	void pregenerateRandomVectors(size_t nSamples)
	{
		RandomGenerator random;
		randomVectors.resize(nSamples);

		for(size_t i = 0; i < nSamples; ++i)
		{
			randomVectors[i] = random.unit_vector();
		}
	}

	template<class Op>
	Image* traverseLatLongRand(const Op& op) const
	{
		RandomGenerator random;
		// Actual traverse
		auto resultImage = new Image(nx, ny);
		for(int i = 0; i < ny; ++i)
		{
			float v = 1-float(i)/ny;
			for(int j = 0; j < nx; ++j)
			{
				float u = float(j)/nx;
				auto dir = latLong2Sphere(u, v);
				resultImage->at(j,i) = op(random, dir);
			}
		}
		return resultImage;
	}

	Image* irradianceLambert(int nSamples)
	{
		return traverseLatLongRand([this,nSamples](auto& random, auto& irradianceDir){
			return convolveRadiance(random, nSamples, irradianceDir);
		});
	}

	// Pixar's method for orthonormal basis generation
	void branchlessONB(const Vec3f &n, Vec3f &b1, Vec3f &b2)
	{
		float sign = copysignf(1.0f, n.z());
		const float a = -1.0f / (sign + n.z());
		const float b = n.x() * n.y() * a;
		b1 = Vec3f(1.0f + sign * n.x() * n.x() * a, sign * b, -sign * n.x());
		b2 = Vec3f(b, sign + n.y() * n.y() * a, -n.y());
	}

	Vec3f ImportanceSampleGGX( Vec2f Xi, float Roughness, Vec3f N )
	{
		float a = Roughness * Roughness;
		float Phi = 2 * Pi * Xi.x();
		float CosTheta = sqrt( (1 - Xi.y()) / ( 1 + (a*a - 1) * Xi.y() ) );
		float SinTheta = sqrt( 1 - CosTheta * CosTheta );
		Vec3f H;
		H.x() = SinTheta * cos( Phi );
		H.y() = SinTheta * sin( Phi );
		H.z() = CosTheta;

		return H;
	}

	const Vec3f& sampleSpherical(const Vec3f& dir) const
	{
		auto uv = sphere2LatLong(dir);
		auto sx = uv.x() * (nx-1);
		auto sy = uv.y() * (ny-1);
		return at((int)sx, (int)sy);
	}

	Vec3f PrefilterEnvMap(size_t numSamples, float Roughness, Vec3f R )
	{
		Vec3f V = R;
		Vec3f N = R;
		// Compute an orthonormal basis
		Vec3f tangent, bitangent;
		branchlessONB(N, tangent, bitangent);

		// Convolve filter
		Vec3f PrefilteredColor = Vec3f(0.f, 0.f, 0.f);
		float TotalWeight = 0.f;
		for( size_t i = 0; i < numSamples; i++ )
		{
			Vec2f Xi = randomSamples[i];
			Vec3f h = ImportanceSampleGGX( Xi, Roughness, N );
			Vec3f H = tangent * h.x() + bitangent * h.y() + N * h.z();
			Vec3f L = 2 * dot(V,H) * H - V;
			float NoL = min(1.f, dot(N,L) );
			if( NoL > 0 )
			{
				PrefilteredColor = PrefilteredColor + sampleSpherical(L) * NoL;
				TotalWeight += NoL;
			}
		}
		return PrefilteredColor * (1/TotalWeight);
	}

	Image* radianceGGX(size_t nSamples, float r, int dstNx, int dstNy)
	{
		pregenerateRandomSamples(nSamples);

		// Actual traverse
		auto resultImage = new Image(dstNx, dstNy);
		for(int i = 0; i < dstNy; ++i)
		{
			float v = 1-float(i)/dstNy;
			for(int j = 0; j < dstNx; ++j)
			{
				float u = float(j)/dstNx;
				auto dir = latLong2Sphere(u, v);
				resultImage->at(j,i) = PrefilterEnvMap(nSamples, r, dir);
			}
		}

		return resultImage;
	}

	Vec3f* m;
	int nx, ny;
	std::vector<Vec2f> randomSamples;
	std::vector<Vec3f> randomVectors;
};

//----------------------------------------------------------------------------------------------------------------------
Image* loadImage(string& fileName)
{
	// Load source data
	int nx, ny, nc;

	auto rawData = stbi_loadf(fileName.c_str(), &nx, &ny, &nc, 3);
	if(!rawData)
		return nullptr;
	
	auto img = new Image(nx, ny);
	for(int i = 0; i < img->nPixels(); ++i)
		img->at(i) = reinterpret_cast<Vec3f&>(rawData[3*i]);

	stbi_image_free(rawData);

	return img;
}

string commonPBRCode = R"(
const float PI = 3.1415927410125732421875;
const float TWO_PI = 6.283185307179586;
const float HALF_PI = 1.5707963267948966;
const float INV_PI = 0.3183098861837907;

//------------------------------------------------------------------------------	
void branchlessONB(vec3 n, out vec3 b1, out vec3 b2)
{
	float sign = (n.z>=0)?1.0:-1.0;
	float a = -1.0f / (sign + n.z);
	float b = n.x * n.y * a;
	b1 = vec3(1.0 + sign * n.x * n.x * a, sign * b, -sign * n.x);
	b2 = vec3(b, sign + n.y * n.y * a, -n.y);
}

//------------------------------------------------------------------------------
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

//------------------------------------------------------------------------------
vec2 Hammersley(float i, float N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(uint(i)));
}

//------------------------------------------------------------------------------	
vec3 ImportanceSampleGGX(vec2 Xi, float roughness)
{
    float a = roughness*roughness;
	
    float phi = TWO_PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    return H;
}
)";

string fullPassVtxShader = R"(
#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;

out vec2 latLong;

//------------------------------------------------------------------------------
void main ( void )
{
	latLong = vec2(PI,0.5*PI) * (vertex.xy + 1);
	gl_Position = vec4(vertex.xy, 1.0, 1.0);
}
#endif
)";

//----------------------------------------------------------------------------------------------------------------------
void generateProbeFromImage(const Params& params, Device& device, rev::graphics::Image* srcImg)
{
	Json probeDesc;
	probeDesc["mips"] = Json::array();
	auto& mipsDesc = probeDesc["mips"];

	// Load latlong texture into the GPU
	// Create a generic sampler for latlong images
	auto latLongSamplerDesc = TextureSampler::Descriptor();
	latLongSamplerDesc.wrapT = TextureSampler::Descriptor::Wrap::Clamp; // Clamp to edge on the vertical axis, since the poles of a latlong image don't match
	auto latLongSampler = device.createTextureSampler(latLongSamplerDesc);
	// Create a descriptor for latlong textures
	Texture2d::Descriptor latLongDesc;
	latLongDesc.sampler = latLongSampler;
	latLongDesc.channelType = Texture2d::Descriptor::ChannelType::Float32;
	latLongDesc.pixelFormat = Texture2d::Descriptor::PixelFormat::RGBA;
	// Info specific for this image
	latLongDesc.srcImages = &srcImg;
	latLongDesc.mipLevels = 1;
	latLongDesc.size = srcImg->size();
	// Allocate and init the texture
	auto srcLatLong = device.createTexture2d(latLongDesc);
	//glBindTexture(GL_TEXTURE_2D, 0);

	// --- Create source cubemap ---
	// Create the cubemap texture
	GLuint srcCubeMap;
	glGenTextures(1, &srcCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, srcCubeMap);
	GLenum cubemapTextures[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	int cubeSize = 512; // TODO: This must be parametric, or a function of source image's size
	for(auto target : cubemapTextures)
	{
		glTexImage2D(target, 0, GL_RGBA32F, cubeSize, cubeSize, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // TODO: Trilinear
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// TODO: Max LOD

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	// Create a frame buffer using the cubemap
	GLuint cubeFrameBuffer;
	glGenFramebuffers(1, &cubeFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, cubeFrameBuffer);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// TODO: Maybe use MRT to draw the six faces of the cube at once. Generate 6 sets of uvs in the vertex shader
	// Render to the texture
	// Set up source texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcLatLong.id);
	glViewport(0,0,cubeSize,cubeSize);

	// Create shader
	string shaderCode = R"(
#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;

layout(location = 0) uniform mat4 viewProj;

out vec3 vtxViewDir;

//------------------------------------------------------------------------------
void main ( void )
{
	mat4 invViewProj = inverse(viewProj);
	// Direction from the view point to the pixel, in world space
	vtxViewDir = (invViewProj * vec4(vertex.xy, 1.0, 1.0)).xyz; 
	gl_Position = vec4(vertex.xy, 1.0, 1.0);
}
#endif

#ifdef PXL_SHADER
out highp vec3 outColor;
in vec3 vtxViewDir;

// Global state
layout(location = 1) uniform sampler2D uLatLongMap;


//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSpherical(vec3 v)
{
	vec2 uv = vec2(atan(v.x, -v.z), acos(-v.y));
	uv *= invAtan;
	uv.x += 0.5;
	return uv;
}

//------------------------------------------------------------------------------	
void main (void) {
	vec3 color = textureLod(uLatLongMap, sampleSpherical(normalize(vtxViewDir)), 0.0).xyz;
	
	outColor = color;
}

#endif
)";
	auto lat2CubeShader = rev::graphics::Shader::createShader(shaderCode.c_str());
	auto quad = rev::graphics::RenderGeom::quad({2.f,2.f});

	// Bind shader
	lat2CubeShader->bind();
	auto proj = rev::math::frustumMatrix(HalfPi, 1.f, 0.5f, 10.f);
	
	// Set up framebuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, cubeFrameBuffer);
	rev::math::Quatf rotations[] = 
	{
		rev::math::Quatf({0.f,1.f,0.f}, -HalfPi),
		rev::math::Quatf({0.f,1.f,0.f}, HalfPi),
		//rev::math::Quatf({1.f,0.f,0.f}, HalfPi),
		//rev::math::Quatf({1.f,0.f,0.f}, -HalfPi),
		rev::math::Quatf(normalize(Vec3f(0.f,1.f,1.f)), Pi),
		rev::math::Quatf(normalize(Vec3f(0.f,-1.f,1.f)), Pi),
		rev::math::Quatf({0.f,1.f,0.f}, Pi),
		rev::math::Quatf({0.f,1.f,0.f}, 0.f)
	};
	for(int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, srcCubeMap, 0);

		// Bind uniforms
		Mat44f view = Mat44f::identity();
		view.block<3,3>(0,0) = Mat33f(rotations[i]);
		auto viewProj = proj * view;
		glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, reinterpret_cast<const float*>(&viewProj));
		glUniform1i(1, 0); // Texture stage 0 into uniform 1
		
		// Render the quad
		glBindVertexArray(quad.getVao());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

		// For debugging purposes
		// Image* cubeImg = new Image(cubeSize, cubeSize);
		// glFinish();
		// glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, GL_FLOAT, cubeImg->m);
		// stringstream ss; ss << "cube" << i << ".png";
		// cubeImg->save2sRGB(ss.str());
	}

	// Generate mipmaps from cubemap
	glGenerateTextureMipmap(srcCubeMap);

	// Filter radiance
	vector<const char*> radianceShaderCode = {
		commonPBRCode.c_str(),
		fullPassVtxShader.c_str(),
		R"(
#ifdef PXL_SHADER
out highp vec3 outColor;
in vec2 latLong;

// Global state
layout(location = 0) uniform samplerCube uSkybox;
layout(location = 1) uniform float roughness;

//------------------------------------------------------------------------------	
void main (void) {
	float cPhi = cos(latLong.y);
	float sPhi = sqrt(1-cPhi*cPhi);

	// Sample direction
	vec3 normal = vec3(sin(latLong.x)*sPhi,cPhi,cos(latLong.x)*sPhi);
	
	// Get an orthonormal basis
	vec3 tangent;
	vec3 bitangent;
	branchlessONB(normal, tangent, bitangent);

	// Integrate over the hemisphere
	vec3 accum = vec3(0.0);
	vec3 V = normal;
	
	const float nSamples = 10000;
	float totalWeight = 0;
	for(float i = 0; i < nSamples; ++i)
	{
		vec2 Xi = Hammersley(i, nSamples);
		vec3 tsH = ImportanceSampleGGX(Xi, roughness);
		vec3 wsH = tangent * tsH.x + bitangent * tsH.y + normal * tsH.z;

		vec3 L  = normalize(2.0 * dot(V, wsH) * wsH - V);

		float NdotL = max(dot(normal, L), 0.0);
		if(NdotL > 0.0)
		{
			accum += textureLod(uSkybox, L, roughness * 3.0).rgb * NdotL;
			totalWeight += NdotL;
		}
	}

	outColor = accum / totalWeight;
}

#endif
)"};

	// Bind shader
	auto radianceShader = rev::graphics::Shader::createShader(radianceShaderCode);
	radianceShader->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, srcCubeMap);
	glUniform1i(0, 0);
	latLongDesc.srcImages = nullptr;

	size_t nRadianceMips = 5;
	for(int i = 0; i < nRadianceMips; ++i)
	{
		int baseSize = (1<<(8-i));
		Image* cubeImg = new Image(4*baseSize,2*baseSize);
		// Update descriptor's texture size
		latLongDesc.size = { (uint32_t)cubeImg->nx, (uint32_t)cubeImg->ny };
		// Create dst texture
		auto dstRradiance = device.createTexture2d(latLongDesc);

		// Bind irradiance into the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstRradiance.id, 0);
		glViewport(0,0,cubeImg->nx, cubeImg->ny);
		glClearColor(0.f,1.f,1.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Update roughness
		glUniform1f(1, i/(nRadianceMips-1.f));
		// Draw a full screen quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
		glFinish();

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, cubeImg->m);

		stringstream ss;
		ss << params.out << i << ".hdr";
		auto name = ss.str();
		cubeImg->saveHDR(ss.str());

		mipsDesc.push_back({});
		auto& levelDesc = mipsDesc[i];
		levelDesc["size"] = { cubeImg->nx, cubeImg->ny };
		levelDesc["name"] = name;
	}

	// Generate irradiance from cubemap
	Image* cubeImg = new Image(32,16);
	// Update descriptor's texture size
	latLongDesc.size = { (uint32_t)cubeImg->nx, (uint32_t)cubeImg->ny };
	// Create dst texture
	auto dstIrradiance = device.createTexture2d(latLongDesc);

	// Bind irradiance into the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstIrradiance.id, 0);
	glViewport(0,0,cubeImg->nx, cubeImg->ny);
	glClearColor(0.f,1.f,0.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	vector<const char*> irradianceCode = {
		commonPBRCode.c_str(),
		fullPassVtxShader.c_str(),
		R"(
#ifdef PXL_SHADER
out highp vec3 outColor;
in vec2 latLong;

// Global state
layout(location = 0) uniform samplerCube uSkybox;

//------------------------------------------------------------------------------	
void main (void) {
	float cPhi = cos(latLong.y);
	float sPhi = sqrt(1-cPhi*cPhi);

	// Sample direction
	vec3 normal = vec3(sin(latLong.x)*sPhi,cPhi,cos(latLong.x)*sPhi);
	
	// Get an orthonormal basis
	vec3 tangent;
	vec3 bitangent;
	branchlessONB(normal, tangent, bitangent);

	// Integrate over the hemisphere
	vec3 accum = vec3(0.f);
	
	const float nTheta = 100;
	const float nPhi = 100;
	for(float i = 0; i < nTheta; ++i)
	{
		vec3 sliceAccum = vec3(0.0); // Slice accum for improved numerical behavior
		float theta = i * TWO_PI / nTheta;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);
		for(float j = 0; j < nPhi; ++j)
		{
			float phi = j * HALF_PI/nPhi;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			vec3 sampleDir = 
				tangent * cosTheta * sinPhi +
				bitangent * sinTheta * sinPhi +
				normal * cosPhi;

			sliceAccum += textureLod(uSkybox, sampleDir, 4.0).xyz * cosPhi * sinPhi;
		}
		accum += sliceAccum / nPhi;
	}

	outColor = TWO_PI * accum / nTheta;
}

#endif
)" };
	// Bind shader
	auto irradianceShader = rev::graphics::Shader::createShader(irradianceCode);
	irradianceShader->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, srcCubeMap);
	glUniform1i(0, 0);

	// Draw a full screen quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	glFinish();

	stringstream ss;
	ss << params.out << nRadianceMips << ".hdr";
	auto name = ss.str();

	mipsDesc.push_back({});
	auto& levelDesc = mipsDesc.back();
	levelDesc["size"] = { cubeImg->nx, cubeImg->ny };
	levelDesc["name"] = name;
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, cubeImg->m);
	cubeImg->saveHDR(name);
	// Save results to disk

	ofstream(params.out + ".json") << mipsDesc.dump(4);
}

//----------------------------------------------------------------------------------------------------------------------
void generateIblLut(const Params& params, Device& device)
{
	Texture2d::Descriptor textureDesc;
	textureDesc.size = { 512u, 512u };
	textureDesc.sampler = device.createTextureSampler(TextureSampler::Descriptor());
	textureDesc.channelType = Texture2d::Descriptor::ChannelType::Byte;
	textureDesc.pixelFormat = Texture2d::Descriptor::PixelFormat::RGBA;
	textureDesc.sRGB = false;
	auto texture = device.createTexture2d(textureDesc);

	vector<const char*> shaderCode = {
		commonPBRCode.c_str(),
		fullPassVtxShader.c_str(),
		R"(
#ifdef PXL_SHADER
out lowp vec3 outColor;
in vec2 latLong;

//------------------------------------------------------------------------------
vec2 IntegrateBRDF( float roughness, float ndv )
{
	vec3 V;
	V.x = sqrt( 1.0f - ndv * ndv ); // sin
	V.y = 0;
	V.z = ndv; // cos

	float A = 0;
	float B = 0;

	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		vec2 Xi = Hammersley( i, NumSamples );
		vec3 H = ImportanceSampleGGX( Xi, roughness);
		vec3 L = 2 * dot( V, H ) * H - V;

		float ndl = clamp( L.z , 0.0, 1.0);
		float NoH = H.z;
		float VoH = clamp( dot( V, H ) , 0.0, 1.0 );

		if( ndl > 0 )
		{
			// GGX Geometry schlick
			float k = roughness*roughness/2;
			float g1V = ndv/(ndv*(1-k)+k);
			float g1L = ndl/(ndl*(1-k)+k);
			float G = g1V*g1L;

			float G_Vis = G * VoH / (NoH * ndv);
			float Fc = pow( 1 - VoH, 5 );
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return vec2( A, B ) / NumSamples;
}
//------------------------------------------------------------------------------	
void main (void) {

	float ndv = gl_FragCoord.x/512;
	float roughness = gl_FragCoord.y/512;

	vec2 brdf = IntegrateBRDF(roughness, ndv);

	outColor = vec3(brdf.x, brdf.y, 0.0);
	//outColor = vec3(brdf.x);
}

#endif
)"};


	auto shader = rev::graphics::Shader::createShader(shaderCode);
	shader->bind();
	auto quad = rev::graphics::RenderGeom::quad({2.f,2.f});
	// Create a frame buffer using the cubemap
	GLuint cubeFrameBuffer;
	glGenFramebuffers(1, &cubeFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, cubeFrameBuffer);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	// Bind irradiance into the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.id, 0);
	glViewport(0,0,512,512);
	glClearColor(0.f,1.f,0.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw a full screen quad
	glBindVertexArray(quad.getVao());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	glFinish();

	Image* lut = new Image(512,512);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, lut->m);
	//lut->save2sRGB(params.out);
	lut->saveHDR(params.out);
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Parse arguments
	Params params;
	if (!params.parseArguments(_argc, _argv))
		return -1;

	// Create a grapics device, so we can use all openGL features
	rev::core::OSHandler::startUp();
	rev::gfx::DeviceOpenGLWindows device;

	if(params.in.empty())
	{
		// Generate brdf LUT
		if(params.out.empty())
			params.out = "ibl.hdr";

		generateIblLut(params, device);
		return 0;
	}

	if(params.out.empty())
	{
		params.out = params.in;
	}

	// Load source data
	auto srcImg = rev::graphics::Image::load(params.in, 3);
	//auto srcImg = Image::constantImage(360, 180, 0.5f); // Energy conservation test

	if(!srcImg)
	{
		cout << "Error: Unable to load input image\n";
		return -1;
	}

	generateProbeFromImage(params, device, &*srcImg);

	return 0;
}
