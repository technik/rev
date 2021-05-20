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
#include <math/linear.h>
#include <math/noise.h>
#include <nlohmann/json.hpp>

#include <core/platform/osHandler.h>
#include <core/platform/cmdLineParser.h>
#include <graphics/Image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include "Heitz/MicrosurfaceScattering.h"

// Convert latlong images into environment probes

using namespace std;
using namespace rev::math;
using namespace rev::gfx;
using Json = nlohmann::json;

struct Params {
	std::string in;
	std::string out;
	bool generateBRDFLUT = false;

#ifdef _WIN32
	static constexpr size_t arg0 = 1;
#else
	static constexpr size_t arg0 = 0;
#endif

	bool parseArguments(int _argc, const char** _argv)
	{
		rev::core::CmdLineParser parser;
		parser.addOption("in", &in);
		parser.addOption("out", &out);
		parser.addFlag("brdfLut", generateBRDFLUT);
		parser.parse(_argc, _argv);

		if (out.empty())
			out = generateBRDFLUT ? "ibl.hdr" : in;

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

void saveHDR(const Image& img, const std::string& fileName)
{
	if(extension(fileName) != "hdr")
	{
		cout << "Only .hdr is supported for hdr output images\n";
	}

	assert(img.format() == vk::Format::eR32G32B32Sfloat);
	auto src = reinterpret_cast<const float*>(img.data());
	stbi_write_hdr(fileName.c_str(), img.width(), img.height(), 3, src);
}

void save2sRGB(const Image& img, const std::string& fileName)
{
	if(extension(fileName) != "png")
	{
		cout << "Only png is supported for output images\n";
	}
	auto nBytes = 3*img.area();
	std::vector<uint8_t> raw(nBytes);
	assert(img.format() == vk::Format::eR32G32B32Sfloat);
	auto src = reinterpret_cast<const float*>(img.data());

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
	auto bytesPerRow = 3 * img.width();
	stbi_write_png(fileName.c_str(), img.width(), img.height(), 3, raw.data(), bytesPerRow);
}

void saveLinear(const Image& img, const std::string& fileName)
{
	if(extension(fileName) != "png")
	{
		cout << "Only png is supported for output images\n";
	}
	auto nBytes = 3*img.area();
	std::vector<uint8_t> raw(nBytes);
	assert(img.format() == vk::Format::eR32G32B32Sfloat);
	auto src = reinterpret_cast<const float*>(img.data());

	// Linear to sRGB conversion following the formal specification of sRGB
	for(int i = 0; i < nBytes; ++i)
	{
		auto linear = src[i] * 255.f;
		raw[i] = (uint8_t)clamp(linear, 0.f, 255.f);
	}
	auto bytesPerRow = 3*img.width();
	stbi_write_png(fileName.c_str(), img.width(), img.height(), 3, raw.data(), bytesPerRow);
}

// Integrate Fresnel modulated directional albedo components
// res = directionalFresnel(r, ndv, N);
// where ndv = dot(normal, w), then
// Ess(w) = res.x+res.y is the directional albedo and
// Fss(w) = F0 * res.x + res.y is the Fresnel modulated directional
// albedo usig a Schlick approximated Fresnel.
Vec2f directionalFresnel(float roughness, float ndv, uint32_t numSamples)
{
	// Handle special cases
	if (roughness == 0)
	{
		if (ndv == 0)
			return { 0.f, 1.f };
		if (ndv == 1)
			return { 1.f, 0.f };
		// F = F0 + (1-F0)(1-CosT)^5
		// F = F0 + (1-F0)(1-ndv)^5
		// F = F0 + (1-ndv)^5 - F0(1-ndv)^5
		// F = F0(1-(1-ndv)^5) + (1-ndv)^5
		float one_min_ndv = 1 - ndv;
		float B = one_min_ndv * one_min_ndv * one_min_ndv * one_min_ndv * one_min_ndv;
		float A = 1 - B;
		return Vec2f(A, B);
	}

	Vec3f V;
	V.x() = sqrt(1.0f - ndv * ndv); // sin
	V.y() = 0;
	V.z() = ndv; // cos

	float A = 0;
	float B = 0;

	if (roughness == 1)
	{
		for (uint32_t i = 0; i < numSamples; i++)
		{
			Vec2f Xi = Hammersley(i, numSamples);
			Vec3f H = ImportanceSampleGGX_r1(Xi);
			Vec3f L = 2.f * dot(V, H) * H - V;

			float ndl = clamp(L.z(), 0.f, 1.f);
			float NoH = H.z();
			float VoH = clamp(dot(V, H), 0.f, 1.f);

			if (ndl > 0) // TODO: Re-write this using the distribution of visible normals
			{
				// GGX Geometry schlick
				float G2_over_ndv = 2 * ndl / (ndl + ndv);

				float G_Vis = G2_over_ndv * VoH / (NoH);
				float Fc = pow(1 - VoH, 5);
				A += (1 - Fc) * G_Vis;
				B += Fc * G_Vis;
			}
		}
	}
	else
	{
		float alpha = roughness * roughness;

		for (uint32_t i = 0; i < numSamples; i++)
		{
			Vec2f Xi = Hammersley(i, numSamples);
			Vec3f H = ImportanceSampleGGX(Xi, roughness);
			Vec3f L = 2 * dot(V, H) * H - V;

			float ndl = clamp(L.z(), 0.f, 1.f);
			float NoH = H.z();
			float VoH = clamp(dot(V, H), 0.f, 1.f);

			if (ndl > 0) // TODO: Re-write this using the distribution of visible normals
			{
				// GGX Geometry schlick
				float G2;
				if(ndv == 0)
					G2 = SmithGGXCorrelatedG2_overNdv_ndv0(alpha);
				else
					G2 = SmithGGXCorrelatedG2_over_ndv(ndv, ndl, alpha);

				float G_Vis = G2 * VoH / NoH;
				float Fc = pow(1 - VoH, 5);
				A += (1 - Fc) * G_Vis;
				B += Fc * G_Vis;
			}
		}
	}
	return Vec2f(A / numSamples, B / numSamples);
}

float directionalAlbedo(float roughness, float ndv, uint32_t numSamples)
{
	Vec2f dirFresnel = directionalFresnel(roughness, ndv, numSamples);
	return dirFresnel.x() + dirFresnel.y();

}

void generateIBLCPU()
{
	constexpr uint32_t lutSize = 64;
	constexpr int numSamples = 512;
	auto lut = std::make_shared<Image>(vk::Format::eR32G32B32Sfloat, Vec2u{ lutSize, lutSize });

	for (int i = 0; i < lutSize; ++i)
	{
		float r = i / float(lutSize - 1);
		for (int j = 0; j < lutSize; ++j)
		{
			float ndv = j / float(lutSize - 1);

			Vec2f fresnelTerms = directionalFresnel(r, ndv, numSamples);
			lut->pixel<Vec3f>(j, i).x() = fresnelTerms.x();
			lut->pixel<Vec3f>(j, i).y() = fresnelTerms.y();
			lut->pixel<Vec3f>(j, i).z() = 0;
		}
	}

	saveLinear(*lut, "ibl.png");
	//lut->saveHDR("ibl.hdr");

}

std::shared_ptr<Image> renderSphere(const Vec2u& size, float radius)
{
	auto img = std::make_shared<Image>(vk::Format::eR32G32B32Sfloat, size);
	Vec2f center(float(size.x()) / 2, float(size.y()) / 2);

	for (uint32_t i = 0; i < size.y(); ++i)
	{
		for (uint32_t j = 0; j < size.x(); ++j)
		{
			Vec2f samplePos2d(j, i);
			auto relPos2d = samplePos2d - center;
			if (norm(relPos2d) < radius)
				img->pixel<Vec3f>(i, j) = Vec3f::ones();
			else
				img->pixel<Vec3f>(i, j) = Vec3f::zero();
		}
	}

	return img;
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Parse arguments
	Params params;
	if (!params.parseArguments(_argc, _argv))
		return -1;

	// Create a grapics device, so we can use all openGL features
	rev::core::OSHandler::startUp();
	
	// Generate a r=0.5 GGX microsurface sphere
	auto render = renderSphere({ 512,512 }, 200.f);
	save2sRGB(*render, "render.png");

	return 0;
}
