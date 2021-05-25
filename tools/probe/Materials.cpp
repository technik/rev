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
#include <math/noise.h>
#include <numbers>


using namespace std::numbers;
using namespace rev::math;

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
				Vec2f Xi = Hammersley(i, numSamples) + Vec2f(0.5f / numSamples, 0.5f / numSamples);
				Vec3f H = ImportanceSampleGGX_r1(Xi);
				Vec3f L = 2.f * dot(V, H) * H - V;

				float ndl = rev::math::clamp(L.z(), 0.f, 1.f);
				float NoH = H.z();
				float VoH = rev::math::clamp(dot(V, H), 0.f, 1.f);

				if (ndl > 0) // TODO: Re-write this using the distribution of visible normals
				{
					// GGX Geometry schlick
					float G2_over_ndv = 2 * ndl / (ndl + ndv);

					float G_Vis = G2_over_ndv * VoH / (NoH);
					float Fc = powf(1 - VoH, 5);
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
				Vec2f Xi = Hammersley(i, numSamples) + Vec2f(0.5f / numSamples, 0.5f / numSamples);
				Vec3f H = ImportanceSampleGGX(Xi, roughness);
				Vec3f L = 2 * dot(V, H) * H - V;

				float ndl = rev::math::clamp(L.z(), 0.f, 1.f);
				float NoH = H.z();
				float VoH = rev::math::clamp(dot(V, H), 0.f, 1.f);

				if (ndl > 0) // TODO: Re-write this using the distribution of visible normals
				{
					// GGX Geometry schlick
					float G2;
					if (ndv == 0)
						G2 = SmithGGXCorrelatedG2_overNdv_ndv0(alpha);
					else
						G2 = SmithGGXCorrelatedG2_over_ndv(ndv, ndl, alpha);

					float G_Vis = G2 * VoH / NoH;
					float Fc = powf(1 - VoH, 5);
					A += (1 - Fc) * G_Vis;
					B += Fc * G_Vis;
				}
			}
		}
		return Vec2f(A / numSamples, B / numSamples);
	}

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

Vec3f GGXSmithConductor::brdf(
	const rev::math::Vec3f& eye,
	const rev::math::Vec3f& light,
	const rev::math::Vec3f& half) const
{
	return pureMirrorBRDF(half.z(), light.z(), max(1e-6f,eye.z()), m_roughness);
}

Vec3f HillConductor::brdf(
	const rev::math::Vec3f& eye,
	const rev::math::Vec3f& light,
	const rev::math::Vec3f& half) const
{
	Vec3f s0 = 0.f;
	Vec3f ms = 0.f;
	if (m_scatteringOrder <= 1)
	{
		Vec3f F = m_F0 + (Vec3f(1) - m_F0) * powf(eye.z(), 5);
		s0 = F * pureMirrorBRDF(half.z(), light.z(), max(1e-6f, eye.z()), m_roughness);
	}
	if (m_scatteringOrder == 0 && m_roughness > 0.1f) // Multiple scattering
	{
		Vec2f Eo3 = directionalFresnel(m_roughness, eye.z(), 64);
		float Eo = Eo3.x() + Eo3.y();
		Vec2f Ei3 = directionalFresnel(m_roughness, light.z(), 64);
		float Ei = Ei3.x() + Ei3.y();
		float den = pi_v<float> * compEavg(m_roughness);
		float fms = (1-Eo) * (1 - Ei) / den;

		Vec3f Fms;
	}
	return s0 + ms;
}

vec3 SchlickConductor::evalPhaseFunction(const vec3& wi, const vec3& wo) const
{
	vec3 mirrorPhaseFunction = MicrosurfaceConductor::evalPhaseFunction(wi, wo);
	vec3 F = m_F0 + (vec3(1) - m_F0) * powf(wi.z, 5);

	return F * mirrorPhaseFunction;
}

Vec3f TurquinConductor::brdf(
	const rev::math::Vec3f& eye,
	const rev::math::Vec3f& light,
	const rev::math::Vec3f& half) const
{
	Vec3f mirrorPhaseFunction = HillConductor::brdf(eye, light, half);
	Vec3f F = m_F0 + (Vec3f(1) - m_F0) * powf(eye.z(), 5);

	return F * mirrorPhaseFunction;
}