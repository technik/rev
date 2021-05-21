//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <math/numericTraits.h>
#include <math/linear.h>
#include <math/noise.h>
#include <numbers>
#include <nlohmann/json.hpp>

#include <core/platform/osHandler.h>
#include <core/platform/cmdLineParser.h>
#include <graphics/Image.h>

#include "Materials.h"

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

std::shared_ptr<Image> renderSphere(const Vec2u& size, float radius, float lightClr, const SurfaceMaterial& material)
{
	auto img = std::make_shared<Image>(vk::Format::eR32G32B32Sfloat, size);
	Vec2f center(float(size.x()) / 2, float(size.y()) / 2);

	const auto backgroundColor = Vec3f::ones() * 0.5f;

	Vec3f light = Vec3f(0,1,0);
	Vec3f eye = -Vec3f(0,0,-1);

#ifdef _DEBUG
	constexpr int nSamples = 4;
#else
	constexpr int nSamples = 64;
#endif

	for (uint32_t i = 0; i < size.y(); ++i)
	{
		for (uint32_t j = 0; j < size.x(); ++j)
		{
			Vec2f samplePos2d(j, i);
			Vec3f accumColor = Vec3f::zero();
			for (uint32_t k = 0; k < nSamples; ++k)
			{
				Vec3f pixelColor = backgroundColor;
				Vec2f pixelJitter = Hammersley(k, nSamples);
				Vec2f relPos2d = samplePos2d - center + pixelJitter;
				if (norm(relPos2d) < radius) // Render the sphere
				{
					// Project directions into tangent space
					relPos2d = 1/radius * relPos2d;
					float z = sqrt(1.f - max(0.f,dot(relPos2d, relPos2d)));
					const auto normal = Vec3f (relPos2d.x(), -relPos2d.y(), z);
					Vec3f tan, bitan;
					branchlessONB(normal, tan, bitan);
					Mat33f worldFromTan;
					worldFromTan.col<0>() = tan;
					worldFromTan.col<1>() = bitan;
					worldFromTan.col<2>() = normal;

					Mat33f tanFromWorld = worldFromTan.transpose();

					pixelColor = material.shade(tanFromWorld * eye, tanFromWorld * light) * lightClr * max(0.f, dot(normal,light));
				}

				accumColor = accumColor + pixelColor;
			}
			
			img->pixel<Vec3f>(j, i) = accumColor *(1.f / nSamples);
		}
	}

	return img;
}

std::shared_ptr<Image> renderDisneySlice(SurfaceMaterial& model, int imgSize)
{
	auto image = std::make_shared<Image>(vk::Format::eR32G32B32Sfloat, Vec2u(imgSize, imgSize));
	constexpr int nSamples = 4;

	for (uint32_t i = 0; i < imgSize; ++i)
	{
		for (uint32_t j = 0; j < imgSize; ++j)
		{
			Vec2f pixelPos(j, i);
			Vec3f accumColor = Vec3f::zero();
			for (uint32_t k = 0; k < nSamples; ++k)
			{
				Vec2f pixelJitter = Hammersley(k, nSamples);
				Vec2f samplePos = (pixelPos + pixelJitter) * (1.f/ imgSize);

				// Compute view and light vectors
				float thetaH = samplePos.x() * std::numbers::pi / 2;
				float thetaD = (1-samplePos.y()) * std::numbers::pi / 2;

				//float sinTh = sample
				Vec3f half = Vec3f(sin(thetaH), 0, max(0.f, cos(thetaH))); // Half vector along the xz plane
				Vec3f eye = Vec3f(0, sin(thetaD), 0) + max(0.f, cos(thetaD)) * half;
				Vec3f light = Vec3f(0, -sin(thetaD), 0) + max(0.f, cos(thetaD)) * half;
				Vec3f sampleColor = model.shade(eye, light);

				accumColor = accumColor + sampleColor;
			}

			image->pixel<Vec3f>(j, i) = accumColor * (1.f / nSamples);
		}
	}

	return image;
}

void renderDisneySliceToFile(float r, int scatteringOrder, const std::string& prefix, const std::string& suffix, int imgSize)
{
	auto t0 = std::chrono::system_clock::now();
	std::stringstream ss;
	ss << prefix << int(1000 * r) << suffix;
	std::cout << "Rendering: " << ss.str() << "... ";
	HeitzRoughMirror surface(r * r, scatteringOrder);
	surface.m_numSamples = 64;

	auto render = renderDisneySlice(surface, imgSize);
	auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t0);
	std::cout << (dt.count() / 1000.f) << "s" << std::endl;
	save2sRGB(*render, ss.str());
}

void renderDisneySlices()
{
	const int imageRes = 512;
	for (float r = 0.125f; r <= 1.f; r += 0.125f)
	{
		//renderDisneySliceToFile(r, 0, "disney_conductor_", "_htz.png", imageRes);
	}
	for (float r = 0.125f; r <= 1.f; r += 0.125f)
	{
		renderDisneySliceToFile(r, 1, "disney_conductor_s1_", "_htz.png", imageRes);
	}
	for (float r = 0.125f; r <= 1.f; r += 0.125f)
	{
		renderDisneySliceToFile(r, 2, "disney_conductor_s2_", "_htz.png", imageRes);
	}
}

void renderMetalSpheres()
{
	const float light = 4.f;
	// Generate a r=0.5 GGX microsurface sphere
	const int imageRes = 512;
	for (float r = 0.125f; r <= 1.f; r += 0.125f)
	{
		auto t0 = std::chrono::system_clock::now();
		std::stringstream ss;
		ss << "conductor_" << int(1000 * r) << "_heitz.png";
		std::cout << "Rendering: " << ss.str() << "... ";
		HeitzRoughMirror surface(r * r, 0);

		auto render = renderSphere({ imageRes, imageRes }, imageRes * 0.4f, light, surface);
		auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t0);
		std::cout << (dt.count() / 1000.f) << "s" << std::endl;
		save2sRGB(*render, ss.str());
	}
	for (float r = 0.125f; r <= 1.f; r += 0.125f)
	{
		std::stringstream ss;
		ss << "conductor_s1_" << int(1000 * r) << "_heitz.png";
		std::cout << "Rendering: " << ss.str() << std::endl;
		HeitzRoughMirror surface(r * r, 1);

		auto render = renderSphere({ imageRes, imageRes }, imageRes * 0.4f, light, surface);
		save2sRGB(*render, ss.str());
	}
	for (float r = 0.125f; r <= 1.f; r += 0.125f)
	{
		std::stringstream ss;
		ss << "conductor_s2_" << int(1000 * r) << "_heitz.png";
		std::cout << "Rendering: " << ss.str() << std::endl;
		HeitzRoughMirror surface(r * r, 2);

		auto render = renderSphere({ imageRes, imageRes }, imageRes * 0.4f, light, surface);
		save2sRGB(*render, ss.str());
	}
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Parse arguments
	Params params;
	if (!params.parseArguments(_argc, _argv))
		return -1;

	// Create a grapics device, so we can use all openGL features
	rev::core::OSHandler::startUp();
	
	renderDisneySlices();
	renderMetalSpheres();

	return 0;
}
