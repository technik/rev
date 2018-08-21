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

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// Convert latlong images into environment probes

using namespace std;
using namespace rev::math;
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
		return checkParamsAreValid();
	}

private:
	bool checkParamsAreValid() {
		if (in.empty())
			return false;
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
		for(int n = 0; n < nSamples;)
		{
			// Sample on the hemisphere of incomming radiance
			auto sampleDir = random.unit_vector();
			auto ndv = sampleDir.dot(normal);
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
	void pregenerateRandomSamples(int nSamples)
	{
		randomSamples.resize(nSamples);
		const float deltaX0 = 1.f/nSamples;
		for(size_t i = 0; i < nSamples; ++i)
		{
			randomSamples[i] = Vec2f(i*deltaX0, RadicalInverse_VdC(i));
		}
	}

	// Precompute random samples
	void pregenerateRandomVectors(int nSamples)
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
		return at(sx, sy);
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
			Vec3f L = 2 * V.dot(H ) * H - V;
			float NoL = min(1.f,  N.dot( L ) );
			if( NoL > 0 )
			{
				PrefilteredColor = PrefilteredColor + sampleSpherical(L) * NoL;
				TotalWeight += NoL;
			}
		}
		return PrefilteredColor * (1/TotalWeight);
	}

	Image* radianceGGX(size_t nSamples, float r)
	{
		pregenerateRandomSamples(nSamples);

		// Actual traverse
		auto resultImage = new Image(nx, ny);
		for(int i = 0; i < ny; ++i)
		{
			float v = 1-float(i)/ny;
			for(int j = 0; j < nx; ++j)
			{
				float u = float(j)/nx;
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

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Parse arguments
	Params params;
	if (!params.parseArguments(_argc, _argv))
		return -1;

	if(params.out.empty())
	{
		params.out = params.in + "_radiance";
	}

	// Load source data
	auto srcImg = loadImage(params.in);
	//auto srcImg = Image::constantImage(360, 180, 0.5f); // Energy conservation test

	if(!srcImg)
	{
		cout << "Error: Unable to load input image\n";
		return -1;
	}

	Json probeDesc;
	probeDesc["mips"] = Json::array();
	auto& mipsDesc = probeDesc["mips"];

	// Generate mips
	auto mips = srcImg->generateMipMaps();
	auto nMips = mips.size();

	for(int i = 0; i < nMips; ++i)
	{
		stringstream ss;
		ss << params.out << i << ".hdr";
		auto name = ss.str();
		Image* radiance = mips[i];
		if(radiance == mips.back())
		{
			// Save iradiance in the last mip
			radiance = radiance->irradianceLambert(8000);
		}
		else if(i > 0)
		{
			float roughness = float(i) / (nMips-1);
			radiance = radiance->radianceGGX(1000*i*i, roughness);
		}
		radiance->saveHDR(name);
		// Record mip in desc
		mipsDesc.push_back({});
		auto& levelDesc = mipsDesc[i];
		levelDesc["size"] = { radiance->nx, radiance->ny };
		levelDesc["name"] = name;
	}

	ofstream(params.out + ".json") << mipsDesc.dump(4);

	return 0;
}
