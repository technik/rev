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
#ifndef PBR_FX
#define PBR_FX
// Common pbr code

#if defined(sampler2D_uNormalMap) && defined(VTX_UV_FLOAT) && defined(VTX_TANGENT_FLOAT)
#define VTX_TANGENT_SPACE
#endif

// Common params for both models
layout(location = 13) uniform sampler2D uEmissive;
// TODO: AO Map

#ifdef float_uRoughness
#endif
#ifdef float_uMetallic
#endif
layout(location = 8) uniform sampler2D uEnvBRDF;
#ifdef VTX_TANGENT_SPACE
layout(location = 10) uniform sampler2D uNormalMap;
#endif

#ifdef PXL_SHADER
// Pixel inputs
in vec3 vtxWsNormal;
#ifdef VTX_UV_FLOAT
in vec2 vTexCoord;
#endif
#ifdef VTX_TANGENT_SPACE
in vec4 vtxTangent;
#endif
#endif

#ifdef VTX_TANGENT_SPACE
//------------------------------------------------------------------------------
vec3 getSampledNormal(vec3 tangent, vec3 bitangent, vec3 normal)
{
	vec3 texNormal = (255.f/128.f)*texture(uNormalMap, vTexCoord).xyz - 1.0;
	
	//return normal;
	return normalize(
		tangent*texNormal.x +
		bitangent*texNormal.y +
		normal*max(texNormal.z, 1e-8)
	);
}
#endif

//---------------------------------------------------------------------------------------
vec4 getBaseColor()
{
#if defined(sampler2D_uBaseColorMap) && defined(vec4_uBaseColor)
	vec4 baseColorTex = texture(uBaseColorMap, vTexCoord);
	vec4 baseColor = baseColorTex*uBaseColor;
#else
	#if defined(sampler2D_uBaseColorMap)
		vec4 baseColor = texture(uBaseColorMap, vTexCoord);
		baseColor = vec4(pow(baseColor.r, 2.2), pow(baseColor.g, 2.2), pow(baseColor.b, 2.2), baseColor.a);
	#else
		#if defined(vec4_uBaseColor)
			vec4 baseColor = uBaseColor;
		#else
			vec4 baseColor = vec4(1.0);
		#endif
	#endif
#endif
	return baseColor;
}

struct Physics
{
	float roughness;
	float metallic;
	float ao;
};

Physics getPhysics()
{
	Physics phyParam;
#ifdef sampler2D_uPhysics
	vec3 physics = texture(uPhysics, vec2(vTexCoord.x, vTexCoord.y)).xyz;
	phyParam.roughness = physics.g;
	phyParam.metallic = physics.b;
#else
	#if defined(float_uRoughness)
		phyParam.roughness = uRoughness;
	#else
		phyParam.roughness = 1.0;
	#endif
	#if defined(float_uMetallic)
		phyParam.metallic = uMetallic;
	#else
		phyParam.metallic = 1.0;
	#endif
#endif
	phyParam.ao = 1.0;
#ifdef SpecularSetup
	phyParam.roughness = 1.0 - phyParam.roughness;
#endif
	return phyParam;
}

struct PBRParams
{
	vec4 specular_r; // specular.xyz, ao
	vec4 albedo; // albedo, alpha
};

PBRParams getPBRParams()
{
	PBRParams params;
	vec4 baseColor = getBaseColor();

#ifdef SpecularSetup
	params.specular_r = texture(uPhysics, vec2(vTexCoord.x, vTexCoord.y));
	//params.specular_r.a = 0.50;
	params.albedo.xyz = baseColor.xyz;
#else
	Physics physics = getPhysics();
	const float dielectricF0 = 0.04;
	vec3 F0 = mix(vec3(dielectricF0), baseColor.xyz, physics.metallic);
	params.specular_r = vec4(F0, physics.roughness);
	params.albedo.xyz = baseColor.xyz * (1-physics.metallic);
	params.albedo.a = 1.0;
#endif
	return params;
}


#endif // PBR_FX