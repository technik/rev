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
		float root = alpha / (ndh*ndh*(alpha*alpha-1) + 1);
		return inv_pi_v<float> * root * root;
	}

	// G1(V) = 2ndv / [sqrt(a2 + (1-a2)ndv2) + ndv]
	// Predivided by 2*ndx
	float G1(float ndx, float alpha)
	{
		float a2 = alpha * alpha;
		return 0.5f / (ndx + sqrt(a2 + (1 - a2) * ndx * ndx));
	}

	// Predivided by 4ndv*ndl
	float V_SmithGGXUncorrelated(float ndl, float ndv, float alpha) {

		assert(ndv > 0 || ndl > 0);
		float G1L = G1(ndl,alpha);
		float G1V = G1(ndv, alpha);
		return G1L * G1V;
	}

	// G2(L,V) = (2 ndv ndl) / [ndl sqrt(a2 + (1-a2)ndv2) + ndv sqrt(a2 + (1-a2)ndl2)]
	// Result is predivided by 4*ndl*ndv
	float V_SmithGGXCorrelated(float ndl, float ndv, float alpha) {
		assert(ndv > 0 || ndl > 0);
		float a2 = alpha * alpha;
		float G1L = ndv * sqrt(lerp(ndl * ndl, 1.f, a2));
		float G1V = ndl * sqrt(lerp(ndv * ndv, 1.f, a2));
		return 0.5f / (G1L + G1V);
	}

	// G2(L,V) = (2 ndv ndl) / [ndl a + (1-aa)ndv) + ndv sqrt(a + (1-a)ndl)]
	// Result is predivided by 4*ndl*ndv
	float V_SmithGGXCorrelatedFast(float ndl, float ndv, float alpha) {
		assert(ndv > 0 || ndl > 0);
		float den = lerp(2 * ndl * ndv, ndl + ndv, alpha);
		return 0.5f / den;
	}

	Vec3f F_Schlick(float u, Vec3f f0) {
		float f = pow(1.f - u, 5.f);
		return Vec3f(f) + f0 * (1.f - f);
	}

	float Fd_Lambert() {
		return inv_pi_v<float>;
	}

	// BRDF = D(H)G2(L,V) / (4 ndl ndv)
	float pureMirrorBRDF(float ndh, float ndl, float ndv, float r)
	{
		float alpha = r * r;
		float D = D_GGX(ndh, alpha);
		float G2 = V_SmithGGXCorrelated(ndl, ndv, alpha);
		//float G2 = V_SmithGGXCorrelatedFast(ndl, ndv, alpha);
		return D * G2;
	}

	Vec3f specularBRDF(Vec3f specularColor, float ndh, float ndl, float ndv, float hdv, float r)
	{
		float s = pureMirrorBRDF(ndh, ndv, ndl, r);
		return F_Schlick(hdv, specularColor) * s;
	}

	// See Kulla-Conty 2017, appendix
	// Returns 1-Eavg(r)
	float compEavg(float r)
	{
		constexpr float Aa = 0.592665f;
		constexpr float Ab = -1.47034f;
		constexpr float Ac = 1.47196f;
		float r2 = r * r;
		float r3 = r2 * r;
		return Aa * r3 / (1 + (Ab + Ac * r) * r);
	}
}

Vec3f GGXSmithMirror::brdf(
	const rev::math::Vec3f& eye,
	const rev::math::Vec3f& light,
	const rev::math::Vec3f& half) const
{
	return pureMirrorBRDF(half.z(), light.z(), max(1e-6f,eye.z()), m_roughness);
}

Vec3f KullaContyMirror::brdf(
	const rev::math::Vec3f& eye,
	const rev::math::Vec3f& light,
	const rev::math::Vec3f& half) const
{
	float s0 = 0.f;
	float ms = 0.f;
	if (m_scatteringOrder <= 1)
	{
		s0 = pureMirrorBRDF(half.z(), light.z(), max(1e-6f, eye.z()), m_roughness);
	}
	if (m_scatteringOrder == 0) // Multiple scattering
	{
		Vec3f Eo3 = sIblLut.sample({ eye.z(), m_roughness });
		float Eo = Eo3.x() + Eo3.y();
		Vec3f Ei3 = sIblLut.sample({ light.z(), m_roughness });
		float Ei = Ei3.x() + Ei3.y();
		float den = pi_v<float> *compEavg(m_roughness);
		ms = (1 - Eo) * (1 - Ei) / den;
	}
	return s0 + ms;
}