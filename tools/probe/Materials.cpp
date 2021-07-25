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
#include "Heitz/MicrosurfaceScattering.h"

using namespace std::numbers;
using namespace rev::math;

// Eric Heitz 2018 - Sampling the distribution of visible normals
// Input Ve: view direction
// Input alpha_x, alpha_y: roughness parameters
// Input U1, U2: uniform random numbers
// Output Ne: normal sampled with PDF D_Ve(Ne) = G1(Ve) * max(0, dot(Ve, Ne)) * D(Ne) / Ve.z
vec3 sampleGGXVNDF(vec3 Ve, float alpha_x, float alpha_y, float U1, float U2)
{
    // Section 3.2: transforming the view direction to the hemisphere configuration
    vec3 Vh = normalize(vec3(alpha_x * Ve.x, alpha_y * Ve.y, Ve.z));
    // Section 4.1: orthonormal basis (with special case if cross product is zero)
    float lensq = Vh.x * Vh.x + Vh.y * Vh.y;
    vec3 T1 = lensq > 0 ? vec3(-Vh.y, Vh.x, 0) * glm::inversesqrt(lensq) : vec3(1, 0, 0);
    vec3 T2 = cross(Vh, T1);
    // Section 4.2: parameterization of the projected area
    float r = sqrt(U1);
    float phi = 2.0 * pi * U2;
    float t1 = r * cos(phi);
    float t2 = r * sin(phi);
    float s = 0.5 * (1.0 + Vh.z);
    t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;
    // Section 4.3: reprojection onto hemisphere
    vec3 Nh = t1 * T1 + t2 * T2 + Vh * vec3(sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)));
    // Section 3.4: transforming the normal back to the ellipsoid configuration
    vec3 Ne = normalize(vec3(alpha_x * Nh.x, alpha_y * Nh.y, std::max<float>(0.0, Nh.z)));
    return Ne;
}

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
    ndv = max(ndv, 1e-5f);
	V.x() = sqrt(1.0f - ndv * ndv); // sin
	V.y() = 0;
	V.z() = ndv; // cos

    float brdfBias = 0; // Red channel
    float brdfScale = 0; // Green channel

    float G1V = GeometrySmithGGXPrediv(V.z(), roughness);
    float invG1V = 1.f / G1V;

    float alpha = roughness * roughness;
    for (uint32_t i = 0; i < numSamples; ++i)
    {
        // TODO: Stratify the samples
		Vec2f Xi = Hammersley(i, numSamples) + Vec2f(0.5f / numSamples, 0.5f / numSamples);
        vec3 halfGlm = sampleGGXVNDF(toGlm(V), alpha, alpha, Xi.x(), Xi.y());
        Vec3f half = fromGlm(halfGlm);
        float HdV = max(0,dot(half, V));
        vec3 light = glm::reflect(toGlm(-V), halfGlm);
        if (light.z <= 0)
            continue;
        float G2 = SmithGGXCorrelatedG2Prediv(V.z(), light.z, alpha) * 2 * light.z;
        //float spec = GeometrySmithGGX(light.z, roughness); // use uncorrelated Smith
        float spec = G2 * invG1V; // use correlate Smith

        float fresnelTerm = pow(1 - HdV, 5.f);

        brdfScale += spec * (1 - fresnelTerm);
        brdfBias += spec * fresnelTerm;
    }
    return Vec2f(brdfScale * (1.f / numSamples), brdfBias * (1.f / numSamples));
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

float GGXSmithConductor::ss(
	const vec3& eye,
	const vec3& light,
	const vec3& half) const
{
	return pureMirrorBRDF(half.z, light.z, max(1e-6f, eye.z), m_roughness);
}

vec3 GGXSmithConductor::brdf(
	const vec3& eye,
	const vec3& light,
	const vec3& half) const
{
	float fss = ss(eye, light, half);
	vec3 singleScattering(0);
	vec3 multipleScattering(0);
	if (m_scatteringOrder <= 1)
	{
		vec3 F = m_F0 + (vec3(1) - m_F0) * powf(eye.z, 5);
		singleScattering = F * fss;
	}
	if (m_scatteringOrder == 0 && m_roughness > 0.1f) // Multiple scattering
	{
		multipleScattering = ms(fss,eye,light,half);
	}
	return singleScattering + multipleScattering;
}

vec3 SchlickConductor::evalPhaseFunction(const vec3& wi, const vec3& wo) const
{
	vec3 mirrorPhaseFunction = MicrosurfaceConductor::evalPhaseFunction(wi, wo);
	vec3 F = m_F0 + (vec3(1) - m_F0) * powf(wi.z, 5);

	return F * mirrorPhaseFunction;
}

vec3 HillConductor::ms(
	float fss,
	const vec3& eye,
	const vec3& light,
	const vec3& half) const
{
	Vec2f Eo3 = directionalFresnel(m_roughness, eye.z, 16);
	float Eo = Eo3.x() + Eo3.y();
	Vec2f Ei3 = directionalFresnel(m_roughness, light.z, 16);
	float Ei = Ei3.x() + Ei3.y();
	float compE = compEavg(m_roughness);
	float Eavg = 1 - compE;
	float den = pi_v<float> *compEavg(m_roughness);
	float fms = (1 - Eo) * (1 - Ei) / den;

	vec3 Favg = (20.f * m_F0 + 1.f) / 21.f;
	vec3 Fms = Favg * Favg * Eavg / (1.f - Favg * compE);

	return Fms * fms;
}

vec3 KullaConductor::ms(
	float fss,
	const vec3& eye,
	const vec3& light,
	const vec3& half) const
{
	Vec2f Eo3 = directionalFresnel(m_roughness, eye.z, 16);
	float Eo = Eo3.x() + Eo3.y();
	Vec2f Ei3 = directionalFresnel(m_roughness, light.z, 16);
	float Ei = Ei3.x() + Ei3.y();
	float compE = compEavg(m_roughness);
	float Eavg = 1 - compE;
	float den = pi_v<float> *compEavg(m_roughness);
	float fms = (1 - Eo) * (1 - Ei) / den;

	vec3 Favg = (20.f * m_F0 + 1.f) / 21.f;
	vec3 Fo = Eo3.x() * m_F0 + Eo3.y();
	vec3 Fi = Ei3.x() * m_F0 + Ei3.y();
	vec3 Fms = Favg * Eavg / (1.f - Favg * compE);

	return Fms * fms;
}

vec3 DirectionalConductor::ms(
	float fss,
	const vec3& eye,
	const vec3& light,
	const vec3& half) const
{
	Vec2f Eo3 = directionalFresnel(m_roughness, eye.z, 16);
	float Eo = Eo3.x() + Eo3.y();
	Vec2f Ei3 = directionalFresnel(m_roughness, light.z, 16);
	float Ei = Ei3.x() + Ei3.y();
	float compE = compEavg(m_roughness);
	float Eavg = 1 - compE;
	float den = pi_v<float> *compEavg(m_roughness);
	float fms = (1 - Eo) * (1 - Ei) / den;

	vec3 Favg = (20.f * m_F0 + 1.f) / 21.f;
	vec3 Fo = Eo3.x() * m_F0 + Eo3.y();
	vec3 Fi = Ei3.x() * m_F0 + Ei3.y();
	vec3 Fms = Fi*Favg * Eavg / (1.f - Favg * compE);

	return Fms * fms;
}

vec3 TurquinConductor::ms(
	float fss,
	const vec3& eye,
	const vec3& light,
	const vec3& half) const
{
	//Vec2f Eo3 = directionalFresnel(m_roughness, eye.z, 16);
	//float Eo = Eo3.x() + Eo3.y();
	Vec2f Ei3 = directionalFresnel(m_roughness, light.z, 16);
	float Ei = Ei3.x() + Ei3.y();
	//float compE = compEavg(m_roughness);
	//float Eavg = 1 - compE;
	//float den = pi_v<float> *compEavg(m_roughness);
	//float fms = (1 - Eo) * (1 - Ei) / den;

	//vec3 Favg = (20.f * m_F0 + 1.f) / 21.f;
	//vec3 Fo = Eo3.x() * m_F0 + Eo3.y();
	//vec3 Fi = Ei3.x() * m_F0 + Ei3.y();
	vec3 Fms = m_F0 * (1 - Ei) / Ei;

	return Fms * fss;
}