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

struct HeitzRoughMirror : SurfaceMaterial
{
	MicrosurfaceConductor model;
	int m_scatteringOrder = 0;
#ifdef _DEBUG
	static constexpr int m_numSamples = 4;
#else
	static constexpr int m_numSamples = 4;// 64;
#endif

	HeitzRoughMirror(float roughness, int scattering)
		: model(false, false, roughness* roughness, roughness*roughness)
		, m_scatteringOrder(scattering)
	{
	}

	rev::math::Vec3f brdf(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const override
	{
		float bsdf = 0.f;
		for (int i = 0; i < m_numSamples; ++i)
		{
			float lit = model.eval(toGlm(eye), toGlm(light), m_scatteringOrder);
			bsdf += lit / rev::math::max(1e-6f,light.z());
		}
		return rev::math::Vec3f(bsdf / float(m_numSamples));
	}

	rev::math::Vec3f shade(
		const rev::math::Vec3f& eye,
		const rev::math::Vec3f& light,
		const rev::math::Vec3f& half) const override
	{
		float bsdf = 0.f;
		for (int i = 0; i < m_numSamples; ++i)
		{
			bsdf += model.eval(toGlm(eye), toGlm(light), m_scatteringOrder);
		}
		return rev::math::Vec3f(bsdf / float(m_numSamples));
	}
};

struct GGXSmithMirror : SurfaceMaterial
{
	int m_scatteringOrder = 0;
	float m_roughness = 0;
	float m_alpha = 0;

	GGXSmithMirror(float roughness, int scattering)
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

struct KullaContyMirror : SurfaceMaterial
{
	int m_scatteringOrder = 0;
	float m_roughness = 0;
	float m_alpha = 0;

	inline static rev::gfx::ImageSampler<rev::math::Vec3f> sIblLut;

	KullaContyMirror(float roughness, int scattering)
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
