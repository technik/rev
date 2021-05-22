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

struct SurfaceMaterial
{
	virtual rev::math::Vec3f shade(const rev::math::Vec3f& eye, const rev::math::Vec3f& light) const = 0;
};

__forceinline vec3 toGlm(const rev::math::Vec3f& v)
{
	return vec3(v.x(), v.y(), v.z());
}

struct HeitzRoughMirror : SurfaceMaterial
{
	MicrosurfaceConductor model;
	int m_scatteringOrder = 0;
	int m_numSamples = 16;

	HeitzRoughMirror(float alpha, int scattering)
		: model(false, false, alpha, alpha)
		, m_scatteringOrder(scattering)
	{
	}

	rev::math::Vec3f shade(const rev::math::Vec3f& eye, const rev::math::Vec3f& light) const override
	{
		float lightAccum = 0.f;
		for (int i = 0; i < m_numSamples; ++i)
		{
			lightAccum += model.eval(toGlm(eye), toGlm(light), m_scatteringOrder);
		}
		return rev::math::Vec3f(lightAccum / float(m_numSamples));
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
		, m_alpha(roughness * roughness)
	{
	}

	rev::math::Vec3f shade(const rev::math::Vec3f& eye, const rev::math::Vec3f& light) const override;
};
