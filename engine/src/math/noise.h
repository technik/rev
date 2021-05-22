////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 7th, 2011
////////////////////////////////////////////////////////////////////////////////
// Noise

#pragma once

#include "algebra/vector.h"
#include "numericTraits.h"
#include <random>

namespace rev::math {
	class SNoise
	{
	public:
		// Simplex noise
		//static void		setSimplexSeed(int _seed);
		static float	simplex(float _x, float _y);	// 2D simplex noise
	private:
		static int	fastFloor(double _x);

		static Vec2f	grad2[12];
		static unsigned char p[256];
	};

	class RandomGenerator
	{
	public:
		float scalar()
		{
			return distrib(engine);
		}

		math::Vec3f unit_vector()
		{
			auto theta = math::TwoPi*scalar();
			auto cosPhi = 2*scalar()-1;
			auto sinPhi = sqrt(1-cosPhi*cosPhi);
			return math::Vec3f(
				cos(theta)*sinPhi,
				sin(theta)*sinPhi,
				cosPhi);
		}
	private:
		std::default_random_engine engine;
		std::uniform_real_distribution<float> distrib;
	};

	inline float RadicalInverse_VdC(uint32_t bits)
	{
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) / 0x100000000;
	}

	//------------------------------------------------------------------------------
	inline Vec2f Hammersley(uint32_t i, uint32_t N)
	{
		return Vec2f(float(i) / N, RadicalInverse_VdC(i));
	}

	inline Vec3f ImportanceSampleGGX(Vec2f Xi, float Roughness)
	{
		float a = Roughness * Roughness;
		float Phi = 2 * Pi * Xi.x();
		float CosTheta = sqrt((1 - Xi.y()) / (1 + (a * a - 1) * Xi.y()));
		float SinTheta = sqrt(1 - CosTheta * CosTheta);
		Vec3f H;
		H.x() = SinTheta * cos(Phi);
		H.y() = SinTheta * sin(Phi);
		H.z() = CosTheta;

		return H;
	}

	inline Vec3f ImportanceSampleGGX_r1(Vec2f Xi)
	{
		float Phi = 2 * Pi * Xi.x();
		float CosTheta = sqrt(1 - Xi.y());
		float SinTheta = sqrt(1 - CosTheta * CosTheta);
		Vec3f H;
		H.x() = SinTheta * cos(Phi);
		H.y() = SinTheta * sin(Phi);
		H.z() = CosTheta;

		return H;
	}

	inline float SmithGGXCorrelatedG2(float ndv, float ndl, float alpha)
	{
		float a2 = alpha * alpha;
		float GV = ndv * sqrt(a2 + (1 - a2) * ndl * ndl);
		float GL = ndl * sqrt(a2 + (1 - a2) * ndv * ndv);
		return 2 * ndv * ndl / (GL + GV);
	}

	inline float SmithGGXCorrelatedG2_over_ndv(float ndv, float ndl, float alpha)
	{
		float a2 = alpha * alpha;
		float GV = ndv * sqrt(a2 + (1 - a2) * ndl * ndl);
		float GL = ndl * sqrt(a2 + (1 - a2) * ndv * ndv);
		return 2 * ndl / (GL + GV);
	}


	inline float SmithGGXCorrelatedG2_a1(float ndv, float ndl)
	{
		return 2 * ndv * ndl / (ndl + ndv);
	}

	inline float SmithGGXCorrelatedG2_overNdv_ndv0(float alpha)
	{
		return 2 / alpha;
	}

}	// namespace rev::math
