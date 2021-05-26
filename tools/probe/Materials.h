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
#pragma once

#include "Heitz/MicrosurfaceScattering.h"
#include <math/algebra/vector.h>
#include <graphics/ImageSampler.h>

rev::math::Vec2f directionalFresnel(float roughness, float ndv, uint32_t numSamples);

struct SurfaceMaterial
{
	virtual rev::math::Vec3f brdf(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const = 0;

	virtual rev::math::Vec3f shade(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const
	{
		return light.z() * brdf(eye, light, half);
	}
};

__forceinline vec3 toGlm(const rev::math::Vec3f& v)
{
	return vec3(v.x(), v.y(), v.z());
}

__forceinline rev::math::Vec3f fromGlm(const vec3& v)
{
	return rev::math::Vec3f(v.x, v.y, v.z);
}

class SchlickConductor : public MicrosurfaceConductor
{
public:
	SchlickConductor(const bool height_uniform, // uniform or Gaussian
		const bool slope_beckmann, // Beckmann or GGX
		const float alpha_x,
		const float alpha_y)
		: MicrosurfaceConductor(height_uniform, slope_beckmann, alpha_x, alpha_y)
	{}

public:
	// evaluate local phase function 
	virtual vec3 evalPhaseFunction(const vec3& wi, const vec3& wo) const;

	vec3 m_F0 = { 0.95f, 0.64f, 0.54f };
};

template<class SurfaceModel>
struct HeitzModel : SurfaceMaterial
{
	SurfaceModel model;
	int m_scatteringOrder = 0;
#ifdef _DEBUG
	static constexpr int m_numSamples = 4;
#else
	static constexpr int m_numSamples = 64;
#endif

	HeitzModel(float roughness, int scattering, const rev::math::Vec3f& f0)
		: model(false, false, roughness* roughness, roughness*roughness)
		, m_scatteringOrder(scattering)
	{
	}

	rev::math::Vec3f brdf(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const override
	{
		vec3 bsdf = vec3(0);
		for (int i = 0; i < m_numSamples; ++i)
		{
			vec3 lit = model.eval(toGlm(eye), toGlm(light), m_scatteringOrder);
			bsdf += lit / rev::math::max(1e-6f,light.z());
		}
		vec3 avg = bsdf / float(m_numSamples);
		return rev::math::Vec3f(avg.x, avg.y, avg.z);
	}

	rev::math::Vec3f shade(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const override
	{
		vec3 bsdf = vec3(0);
		for (int i = 0; i < m_numSamples; ++i)
		{
			bsdf += model.eval(toGlm(eye), toGlm(light), m_scatteringOrder);
		}
		return fromGlm(bsdf / float(m_numSamples));
	}
};

using HeitzRoughMirror = HeitzModel<MicrosurfaceConductor>;
using HeitzSchlick = HeitzModel<SchlickConductor>;

struct GGXSmithConductor : SurfaceMaterial
{
	int m_scatteringOrder = 0;
	float m_roughness = 0;
	float m_alpha = 0;
	rev::math::Vec3f m_F0;

	GGXSmithConductor(float roughness, int scattering, rev::math::Vec3f F0)
		: m_scatteringOrder(scattering)
		, m_roughness(roughness)
		, m_alpha(roughness* roughness)
	{
	}

	rev::math::Vec3f brdf(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const override;
};

struct HillConductor : SurfaceMaterial
{
	int m_scatteringOrder = 0;
	float m_roughness = 0;
	float m_alpha = 0;
	rev::math::Vec3f m_F0;

	inline static rev::gfx::ImageSampler<rev::math::Vec3f> sIblLut;

	HillConductor(float roughness, int scattering, rev::math::Vec3f F0)
		: m_scatteringOrder(scattering)
		, m_roughness(roughness)
		, m_alpha(roughness* roughness)
		, m_F0(F0)
	{
	}

	rev::math::Vec3f brdf(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const override;
};

struct TurquinConductor : HillConductor
{
	TurquinConductor(float roughness, int scattering, rev::math::Vec3f F0)
		: HillConductor(roughness, scattering, F0)
	{
	}

	rev::math::Vec3f brdf(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const override;
};