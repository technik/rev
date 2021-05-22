//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "Materials.h"
#include <math/linear.h>
#include <numbers>


using namespace std::numbers;
using namespace rev::math;

// PBR math
namespace {

	float D_GGX(float ndh, float alpha) {
		float a2 = alpha * alpha;
		float root = (ndh * a2 - ndh) * ndh + 1;
		return a2 / (pi  * root * root);
	}

	// G1(V) = 2ndv / [sqrt(a2 + (1-a2)ndv2) + ndv]
	// G2(L,V) = (2 ndv ndl) / [ndl sqrt(a2 + (1-a2)ndv2) + ndv sqrt(a2 + (1-a2)ndl2)]
	// Result is predivided by 4*ndl*ndv
	float V_SmithGGXCorrelated(float ndv, float ndl, float alpha) {
		assert(ndv > 0 || ndl > 0);
		float a2 = alpha * alpha;
		float G1L = ndv * sqrt(lerp(ndl * ndl, 1.f, a2));
		float G1V = ndl * sqrt(lerp(ndv * ndv, 1.f, a2));
		return 0.5 / (G1L + G1V);
	}

	// G2(L,V) = (2 ndv ndl) / [ndl a + (1-aa)ndv) + ndv sqrt(a + (1-a)ndl)]
	// Result is predivided by 4*ndl*ndv
	float V_SmithGGXCorrelatedFast(float NoV, float NoL, float alpha) {
		assert(NoV > 0 || NoL > 0);
		float den = lerp(2 * NoL * NoV, NoL + NoV, alpha);
		return 0.5 / den;
	}

	Vec3f F_Schlick(float u, Vec3f f0) {
		float f = pow(1.f - u, 5.f);
		return Vec3f(f) + f0 * (1.f - f);
	}

	float Fd_Lambert() {
		return 1.0 / pi;
	}

	// BRDF = D(H)G2(L,V) / (4 ndl ndv)
	float pureMirrorBRDF(float ndh, float ndl, float ndv, float r)
	{
		float alpha = r * r;
		float D = D_GGX(ndh, alpha);
		//float G = V_SmithGGXCorrelatedFast(ndv, ndl, alpha);
		float G2 = V_SmithGGXCorrelated(ndv, ndl, alpha);
		return D * G2;
	}

	Vec3f specularBRDF(Vec3f specularColor, float ndh, float ndl, float ndv, float hdv, float r)
	{
		float s = pureMirrorBRDF(ndh, ndv, ndl, r);
		return F_Schlick(hdv, specularColor) * s;
	}
}

Vec3f GGXSmithMirror::shade(
	const rev::math::Vec3f& eye,
	const rev::math::Vec3f& light,
	const rev::math::Vec3f& half) const
{
	return pureMirrorBRDF(half.z(), light.z(), max(1e-6f,eye.z()), m_roughness) *light.z();
}