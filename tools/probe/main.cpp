//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
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
#include "ImageOperations.h"

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



float directionalAlbedo(float roughness, float ndv, uint32_t numSamples)
{
	Vec2f dirFresnel = directionalFresnel(roughness, ndv, numSamples);
	return dirFresnel.x() + dirFresnel.y();

}

void generateIBLCPU()
{
	constexpr uint32_t lutSize = 64;
	constexpr int numSamples = 512;
	auto lut = std::make_shared<Image3f>(Vec2u{ lutSize, lutSize });

	for (int i = 0; i < lutSize; ++i)
	{
		float r = i / float(lutSize - 1);
		for (int j = 0; j < lutSize; ++j)
		{
			float ndv = j / float(lutSize - 1);

			Vec2f fresnelTerms = directionalFresnel(r, ndv, numSamples);
			lut->pixel(j, i).x() = fresnelTerms.x();
			lut->pixel(j, i).y() = fresnelTerms.y();
			lut->pixel(j, i).z() = 0;
		}
	}

	saveLinear(*lut, "ibl.png");
	//lut->saveHDR("ibl.hdr");

}

std::shared_ptr<Image3f> renderHalfSphere(
	const Vec2u& size,
	float radius,
	const Vec3f& lightDir,
	float incidentLight,
	const SurfaceMaterial& materialLeft,
	const SurfaceMaterial& materialRight)
{
	auto img = std::make_shared<Image3f>(size);
	Vec2f center(float(size.x()) / 2, float(size.y()) / 2);

	const auto backgroundColor = vec3(0.5);

	const Vec3f eye = -Vec3f(0,0,-1);
	const Vec3f half = normalize(eye + lightDir);

#ifdef _DEBUG
	constexpr int nSamples = 4;
#else
	constexpr int nSamples = 16;
#endif

	for (uint32_t i = 0; i < size.y(); ++i)
	{
		for (uint32_t j = 0; j < size.x(); ++j)
		{
			Vec2f samplePos2d((float)j, (float)i);
			vec3 accumColor(0);
			for (uint32_t k = 0; k < nSamples; ++k)
			{
				vec3 pixelColor = backgroundColor;
				Vec2f pixelJitter = Hammersley(k, nSamples) + Vec2f(0.5f / nSamples, 0.5f / nSamples);
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

					vec3 tsEye = toGlm(tanFromWorld * eye);
					vec3 tsLight = toGlm(tanFromWorld * lightDir);
					vec3 tsHalf = toGlm(tanFromWorld * half);

					if(j < size.x()/2)
						pixelColor = materialLeft.shade(tsEye, tsLight, tsHalf) * incidentLight * max(0.f, normal.y());
					else
						pixelColor = materialRight.shade(tsEye, tsLight, tsHalf) * incidentLight * max(0.f, normal.y());
				}

				accumColor += pixelColor;
			}
			
			img->pixel(j, i) = fromGlm(accumColor) *(1.f / nSamples);
		}
	}

	return img;
}

std::shared_ptr<Image3f> renderDisneySlice(const SurfaceMaterial& model, uint32_t imgSize)
{
	auto image = std::make_shared<Image3f>(Vec2u(imgSize, imgSize));
	constexpr int nSamples = 4;

	for (uint32_t i = 0; i < imgSize; ++i)
	{
		for (uint32_t j = 0; j < imgSize; ++j)
		{
			Vec2f pixelPos((float)j, (float)i);
			vec3 accumColor(0);
			for (uint32_t k = 0; k < nSamples; ++k)
			{
				Vec2f pixelJitter = Hammersley(k, nSamples) + Vec2f(0.5f / nSamples, 0.5f / nSamples);
				Vec2f samplePos = (pixelPos + pixelJitter) * (1.f/ imgSize);

				// Compute view and light vectors
				float thetaH = samplePos.x() * std::numbers::pi_v<float> / 2;
				float thetaD = (1-samplePos.y()) * std::numbers::pi_v<float> / 2;

				//float sinTh = sample
				vec3 half = vec3(sin(thetaH), 0, max(0.f, cos(thetaH))); // Half vector along the xz plane
				vec3 eye = vec3(0, sin(thetaD), 0) + max(0.f, cos(thetaD)) * half;
				vec3 light = vec3(0, -sin(thetaD), 0) + max(0.f, cos(thetaD)) * half;
				vec3 sampleColor = model.shade(eye, light, half);

				accumColor += accumColor;
			}

			image->pixel(j, i) = fromGlm(accumColor) * (1.f / nSamples);
		}
	}

	return image;
}

void renderDisneySliceToFile(const SurfaceMaterial& surface, float r, const std::string& prefix, const std::string& suffix, uint32_t imgSize)
{
	auto t0 = std::chrono::system_clock::now();
	std::stringstream ss;
	ss << prefix << int(1000 * r) << suffix;
	std::cout << "Rendering: " << ss.str() << "... ";

	auto render = renderDisneySlice(surface, imgSize);
	auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t0);
	std::cout << (dt.count() / 1000.f) << "s" << std::endl;
	save2sRGB(*render, ss.str());
}

template<class SurfaceModel>
void renderDisneySlices(const std::string& suffix, int s0, int sMax)
{
	const int imageRes = 512;
	for (int scatteringOrder = s0; scatteringOrder <= sMax; ++scatteringOrder)
	{
		std::stringstream prefix;
		prefix << "disney_conductor_s" << scatteringOrder << "_";

		for (float r = 0.125f; r <= 1.f; r += 0.125f)
		{
			SurfaceModel surface(r, scatteringOrder, Vec3f(1.f));
			renderDisneySliceToFile(surface, r, prefix.str(), suffix, imageRes);
		}
	}
}

template<class SurfaceModel>
void renderMetalSpheres(const std::string& suffix, int s0, int sMax)
{
	const float light = 4.f;
	// Generate a r=0.5 GGX microsurface sphere
	const int imageRes = 512;
	for (int scatteringOrder = s0; scatteringOrder <= sMax; ++scatteringOrder)
	{
		std::stringstream prefix;
		prefix << "conductor_s" << scatteringOrder << "_";

		for (float r = 0.125f; r <= 1.f; r += 0.125f)
		{
			auto t0 = std::chrono::system_clock::now();
			std::stringstream ss;
			ss << prefix.str() << int(1000 * r) << suffix;
			std::cout << "Rendering: " << ss.str() << "... ";
			SurfaceModel surface(r, scatteringOrder, Vec3f(1.0));

			auto render = renderSphere({ imageRes, imageRes }, imageRes * 0.4f, light, surface, surface);
			auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t0);
			std::cout << (dt.count() / 1000.f) << "s" << std::endl;
			save2sRGB(*render, ss.str());
		}
	}
}

template<class ModelLeft, class ModelRight>
void renderHalfSpheres(const std::string& suffix, int s0, int sMax)
{
	const float light = 4.f;
	// Generate a r=0.5 GGX microsurface sphere
	const int imageRes = 512;
	const vec3 f0(1.f);
	for (int scatteringOrder = s0; scatteringOrder <= sMax; ++scatteringOrder)
	{
		std::stringstream prefix;
		prefix << "conductor_s" << scatteringOrder << "_";

		for (float r = 0.125f; r <= 1.f; r += 0.125f)
		{
			auto t0 = std::chrono::system_clock::now();
			std::stringstream ss;
			ss << prefix.str() << int(1000 * r) << suffix;
			std::cout << "Rendering: " << ss.str() << "... ";
			ModelLeft surfaceLeft(r, scatteringOrder, f0);
			ModelRight surfaceRight(r, scatteringOrder, f0);

			auto render = renderHalfSphere({ imageRes, imageRes }, imageRes * 0.4f, light, surfaceLeft, surfaceRight);
			auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t0);
			std::cout << (dt.count() / 1000.f) << "s" << std::endl;
			save2sRGB(*render, ss.str());
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<class Op>
vector<shared_ptr<Image3f>> iterateRoughness(const Op& op, const vector<float>& roughnessList)
{
	vector<shared_ptr<Image3f>> result;
	result.reserve(roughnessList.size());

	for (auto r : roughnessList)
	{
		result.push_back(op(r));
	}

	return result;
}

//----------------------------------------------------------------------------------------------------------------------
template<class MaterialLeft, class MaterialRight>
std::shared_ptr<Image3f> renderHalfSphere(const uint32_t imgSize, const Vec3f& lightDir, float incidentLight, int scatteringOrder, float r)
{
	const vec3 f0(1.f, 0.5f, 0.0f);
	MaterialLeft surfaceLeft(r, scatteringOrder, f0);
	MaterialRight surfaceRight(r, scatteringOrder, f0);

	std::cout << "Rendering r=" << r << std::endl;
	return renderHalfSphere({ imgSize, imgSize }, 0.4f * imgSize, lightDir, incidentLight, surfaceLeft, surfaceRight);
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Parse arguments
	Params params;
	if (!params.parseArguments(_argc, _argv))
		return -1;

	// Create a grapics device, so we can use all openGL features
	rev::core::OSHandler::startUp();

	// Useful colors
	const vec3 copper(0.95f, 0.64f, 0.54f);

	auto img = renderHalfSphere<HeitzSchlick, HillConductor>(512, { 0,1,0 }, 4.f, 0, 0.5f);
	save2sRGB(*img, "Heitz-Hill.png");
	// Init ibl lut
	//KullaContyMirror::sIblLut.m_image = rev::gfx::Image3f::load("ibl.png");
	/*auto images = iterateRoughness([](auto r) {
		return renderHalfSphere<HeitzSchlick,HillConductor>(512, { 0,1,0 }, 4.f, 0, r);
		}, { 0.125f, 0.5f, 1.f });

	auto imageAccum = std::make_shared<Image3f>(Vec2u(512 * images.size(), 512));
	composeRow(images, *imageAccum);
	save2sRGB(*imageAccum, "Heitz-Hill");*/

	
	//renderHalfSpheres<HeitzRoughMirror, KullaContyMirror>("_Heitz-Kulla.png", 0, 0);
	//renderHalfSpheres<HeitzSchlick, HillConductor>("_Heitz-Schlick.png", 0, 2);
	//renderDisneySlices<GGXSmithMirror>("_GGX.png", 0, 1);
	//renderMetalSpheres<GGXSmithMirror>("_GGX.png", 0, 1);
	//renderDisneySlices<KullaContyMirror>("_KC.png", 0, 0);
	//renderMetalSpheres<KullaContyMirror>("_KC.png", 0, 1);
	//renderDisneySlices<HeitzRoughMirror>("_heitz.png", 0, 3);
	//renderMetalSpheres<HeitzRoughMirror>("_heitz.png", 0, 3);

	return 0;
}
