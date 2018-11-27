//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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

// Metallic-rough pbr shader
#ifdef PXL_SHADER

// Shading params
layout(location = 14) uniform vec4 uBaseColor;
layout(location = 15) uniform float uRoughness;
layout(location = 16) uniform float uMetallic;
// Maps
layout(location = 11) uniform sampler2D uBaseColorMap;
layout(location = 12) uniform sampler2D uPhysics;

#include "pbr.fx"

//---------------------------------------------------------------------------------------
vec4 getBaseColor()
{
	// Default color
	vec4 baseColor = vec4(1.0);
	// scalar factor
	#if defined(vec4_uBaseColor)
		baseColor = uBaseColor;
	#endif
	// Texture
	#if defined(sampler2D_uBaseColorMap)
		baseColor *= texture(uBaseColorMap, vTexCoord);
	#endif
	return baseColor;
}

//---------------------------------------------------------------------------------------
struct Physics
{
	float roughness;
	float metallic;
	float ao;
};

Physics getPhysics()
{
	Physics phyParam;
	// Default params
	phyParam.roughness = 1.0;
	phyParam.metallic = 1.0;
	phyParam.ao = 1.0;
	// floats
	#ifdef float_uRoughness
		phyParam.roughness = uRoughness;
	#endif
	#ifdef float_uMetallic
		phyParam.metallic = uMetallic;
	#endif
	// Texture
#ifdef sampler2D_uPhysics
	vec3 physics = texture(uPhysics, vec2(vTexCoord.x, vTexCoord.y)).xyz;
	phyParam.roughness *= physics.g;
	phyParam.metallic *= physics.b;
#endif
	return phyParam;
}

PBRParams getPBRParams()
{
	PBRParams params;
	vec4 baseColor = getBaseColor();

	Physics physics = getPhysics();
	const float dielectricF0 = 0.04;
	vec3 F0 = mix(vec3(dielectricF0), baseColor.xyz, physics.metallic);
	params.specular_r = vec4(F0, physics.roughness);
	params.albedo.xyz = baseColor.xyz * (1-physics.metallic);
	params.albedo.a = 1.0;

	return params;
}

#endif // PXL_SHADER