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

namespace rev {
	namespace math {
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
	}
}	// namespace rev
