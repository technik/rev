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
	float D_GGX(float NoH, float alpha) {
		float a2 = NoH * alpha;
		float k = alpha / max(1e-4, 1.0 - NoH * NoH + a2 * a2);
		return k * k * (1 / pi);
	}

	float V_SmithGGXCorrelated(float NoV, float NoL, float alpha) {
		float a2 = alpha * alpha;
		float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
		float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
		return 0.5 / max(1e-4, GGXV + GGXL);
	}

	float V_SmithGGXCorrelatedFast(float NoV, float NoL, float alpha) {
		// float GGXV = NoL * (NoV * (1.0 - alpha) + alpha);
		// float GGXL = NoV * (NoL * (1.0 - alpha) + alpha);
		// float den = GGXV + GGXL;
		float den = lerp(2 * NoL * NoV, NoL + NoV, alpha);
		return 0.5 / max(1e-4, den);
	}

	Vec3f F_Schlick(float u, Vec3f f0) {
		float f = pow(1.0 - u, 5.0);
		return Vec3f(f) + f0 * (1.0 - f);
	}

	float Fd_Lambert() {
		return 1.0 / pi;
	}

	float pureMirrorBRDF(float ndh, float ndl, float ndv, float r)
	{
		float D = D_GGX(ndh, r);
		float G = V_SmithGGXCorrelatedFast(ndv, ndl, r);
		return D * G;
	}

	Vec3f specularBRDF(Vec3f specularColor, float ndh, float ndl, float ndv, float hdv, float r)
	{
		float s = pureMirrorBRDF(ndh, ndv, ndl, r);
		return F_Schlick(hdv, specularColor) * s;
	}
}

Vec3f GGXSmithMirror::shade(const Vec3f& eye, const Vec3f& light) const
{
	Vec3f half = normalize(eye + light);
	return pureMirrorBRDF(half.z(), light.z(), eye.z(), m_roughness);
}