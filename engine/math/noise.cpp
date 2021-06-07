////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 7th, 2011
////////////////////////////////////////////////////////////////////////////////
// Noise

#include "noise.h"

#include <cmath>
#include <numbers>

#include <math/algebra/vector.h>

using namespace std::numbers;

namespace rev::math
{
	namespace {
		//------------------------------------------------------------------------------------------------------------------
		Vec2f sSimplexGrad2[12] =
		{
			Vec2f(1.f, 1.f),
			Vec2f(-1.f, 1.f),
			Vec2f(1.f, -1.f),
			Vec2f(-1.f, 1.f),
			Vec2f(1.f, 0.f),
			Vec2f(-1.f, 0.f),
			Vec2f(1.f, 0.f),
			Vec2f(-1.f, 0.f),
			Vec2f(0.f, 1.f),
			Vec2f(0.f, -1.f),
			Vec2f(0.f, 1.f),
			Vec2f(0.f, -1.f)
		};

		//------------------------------------------------------------------------------------------------------------------
		constexpr uint8_t sSimplexPerm512[512] =
		{
			151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
			140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
			247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
			57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175,
			74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
			60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,
			65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
			135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
			5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
			223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
			129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
			251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
			49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
			138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
			// And repeat
			151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
			140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
			247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
			57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175,
			74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
			60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,
			65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
			135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
			5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
			223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
			129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
			251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
			49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
			138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
		};

		float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
		float lerp(float t, float a, float b) { return a + t * (b - a); }
		float grad(int hash, float x, float y, float z)
		{
			// CONVERT LO 4 BITS OF HASH CODE INTO 12 GRADIENT DIRECTIONS.
			int h = hash & 15;
			float u = h < 8 ? x : y;
			float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}
		float grad(int hash, float x, float y)
		{
			// CONVERT LO 2 BITS OF HASH CODE INTO 4 GRADIENT DIRECTIONS.
			return ((hash & 2) == 0) ? (((hash & 1) == 0) ? x : -x) : (((hash & 1) == 0) ? y : -y);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// Simplex noise (Stefan Gustavson, 2005)
	float simplexNoise(float x, float y)
	{
		// Coordinate skew
		constexpr float f2 = 0.5f * float(sqrt3 - 1);
		constexpr float g2 = float(3 - sqrt3) / 6;

		const float skew = f2 * (x + y);
		const int i = fastFloor(x + skew);
		const int j = fastFloor(y + skew);

		float t = (i + j) * g2;
		float X0 = i - t;
		float Y0 = j - t;
		float x0 = x - X0;
		float y0 = y - Y0;

		int i1 = (x0 > y0) ? 1 : 0;
		int j1 = i1 ^ 1;

		float x1 = x0 - i1 + g2;
		float y1 = y0 - j1 + g2;
		float x2 = x0 - 1.f + 2.f*g2;
		float y2 = y0 - 1.f + 2.f*g2;

		int ii = i & 255;
		int jj = j & 255;
		int gi0 = sSimplexPerm512[(ii + sSimplexPerm512[jj]) & 255] % 12;
		int gi1 = sSimplexPerm512[(ii + i1 + sSimplexPerm512[jj + j1]) & 255] % 12;
		int gi2 = sSimplexPerm512[(ii + 1 + sSimplexPerm512[jj + 1])&255] % 12;

		float t0 = 0.5f - x0*x0 - y0*y0;
		float n0, n1, n2; // Noise contributions from the three corners
		if (t0 < 0.f)
		{
			n0 = 0.f;
		}
		else
		{
			t0 *= t0;
			n0 = t0 * t0 * dot(sSimplexGrad2[gi0], Vec2f((float)x0, (float)y0));
		}

		float t1 = 0.5f - x1*x1 - y1*y1;
		if (t1 < 0.f)
		{
			n1 = 0.f;
		}
		else
		{
			t1 *= t1;
			n1 = t1 * t1 * dot(sSimplexGrad2[gi1], Vec2f((float)x1, (float)y1));
		}

		float t2 = 0.5f - x2*x2 - y2*y2;
		if (t2 < 0.f)
		{
			n2 = 0.f;
		}
		else
		{
			t2 *= t2;
			n2 = t2 * t2 * dot(sSimplexGrad2[gi2], Vec2f((float)x2, (float)y2));
		}

		return float(70.f * (n0 + n1 + n2));
	}

	// Perlin noise, 
	// http://mrl.nyu.edu/~perlin/noise/
	float perlinNoise(float x, float y)
	{
		float fx = floorf(x);
		float fy = floorf(y);

		int X = int(fx) & 255;
		int Y = int(fy) & 255;

		x -= fx;
		y -= fy;

		float u = fade(x);
		float v = fade(y);

		int A = sSimplexPerm512[X] + Y;
		int B = sSimplexPerm512[X + 1] + Y;

		// Add contributions from all eight corners
		return lerp(v,
					lerp(u,
						grad(sSimplexPerm512[A], x, y),
						grad(sSimplexPerm512[B], x - 1.f, y - 1.f)),
					lerp(u,
						grad(sSimplexPerm512[A + 1], x, y),
						grad(sSimplexPerm512[B + 1], x - 1.f, y - 1.f)));
	}

	// Perlin noise, 
	// http://mrl.nyu.edu/~perlin/noise/
	float perlinNoise(float x, float y, float z)
	{
		float fx = floorf(x);
		float fy = floorf(y);
		float fz = floorf(z);

		int X = int(fx) & 255;
		int Y = int(fy) & 255;
		int Z = int(fz) & 255;

		x -= fx;
		y -= fy;
		z -= fz;

		float u = fade(x);
		float v = fade(y);
		float w = fade(z);

		int A = sSimplexPerm512[X] + Y;
		int AA = sSimplexPerm512[A] + Z;
		int AB = sSimplexPerm512[A + 1] + Z;
		int B = sSimplexPerm512[X + 1] + Y;
		int BA = sSimplexPerm512[B] + Z;
		int BB = sSimplexPerm512[B + 1] + Z;

		// Add contributions from all eight corners
		return lerp(w,
			lerp(v,
				lerp(u,
					grad(sSimplexPerm512[AA], x, y, z),
					grad(sSimplexPerm512[BA], x - 1.f, y, z)),
				lerp(u,
					grad(sSimplexPerm512[AB], x, y - 1.f, z),
					grad(sSimplexPerm512[BB], x - 1.f, y - 1.f, z))),
			lerp(v,
				lerp(u,
					grad(sSimplexPerm512[AA + 1], x, y, z - 1.f),
					grad(sSimplexPerm512[BA + 1], x - 1.f, y, z - 1.f)),
				lerp(u,
					grad(sSimplexPerm512[AB + 1], x, y - 1.f, z - 1.f),
					grad(sSimplexPerm512[BB + 1], x - 1.f, y - 1.f, z - 1.f))));
	}

}	// namespace rev::math