////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 7th, 2011
////////////////////////////////////////////////////////////////////////////////
// Noise

#ifndef _REV_MATH_NOISE_H_
#define _REV_MATH_NOISE_H_

#include "algebra/vector.h"

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
	}
}	// namespace rev

#endif // _REV_MATH_NOISE_H_