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
//#define Furnace
#define BSDF

#ifdef PXL_SHADER

#ifdef VTX_TANGENT_SPACE
layout(location = 10) uniform sampler2D uNormalMap;
#endif
#ifdef sampler2D_uShadowMap
layout(location = 9) uniform sampler2D uShadowMap;
#endif

#include "pbr.fx"

// Lighting
layout(location = 5) uniform vec3 uLightColor;
layout(location = 6) uniform vec3 uLightDir; // Direction toward light

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

#ifdef sampler2D_uEnvironment
//---------------------------------------------------------------------------------------
vec3 ibl(
  vec3 F0,
  vec3 normal,
  vec3 eye,
  vec3 albedo,
  float roughness,
  float occlusion,
  float shadow,
  float ndv
  )
{	
	// Common code for single and multiple scattering
	vec3 Fr = max(vec3(1.0 - roughness), F0) - F0; // Roughness dependent fresnel
	vec3 kS = F0 + Fr * pow(1.0-ndv, 5.0);

	vec2 f_ab = textureLod(uEnvBRDF, vec2(ndv, roughness), 0).xy;
	vec3 FssEss = kS * f_ab.x + f_ab.y;
	float Ess = f_ab.x + f_ab.y;
	float Ems = 1-Ess;
	Ems = 0.0;

	float lodLevel = roughness * (1.3-0.3*roughness) * numEnvLevels;
	vec3 samplerDir = reflect(-eye, normal);
	vec3 radiance = pow(getRadiance(samplerDir, lodLevel), vec3(1.0)) * mix(occlusion * shadow,1,max(0.0,dot(normal,samplerDir))); // Prefiltered radiance

	//if(radiance.r > 1.0 && radiance.g > 1.0 && radiance.b > 1.0)
	//	radiance.rgb = vec3(1.0, 0.0, 0.0);
	vec3 irradiance = pow(getIrradiance(normal), vec3(1.0)) * occlusion * shadow; // Cosine-weighted irradiance

	//if(irradiance.r > 1.0 && irradiance.g > 1.0 && irradiance.b > 1.0)
	//	irradiance.rgb = vec3(0.0, 1.0, 0.0);
	// Multiple scattering
	vec3 Fms = FssEss*FssEss/(Ess-FssEss*Ems);

	// Dielectrics
	vec3 kD = 1.0 - (FssEss + Fms* Ems);

	// Composition
	return FssEss * radiance + (Fms * Ems + kD*albedo) * irradiance;
}
#endif // sampler2D_uEnvironment

//---------------------------------------------------------------------------------------
vec4 shadeSurface(ShadeInput inputs)
{
	vec4 baseColor = getBaseColor();

#ifdef sampler2D_uPhysics
	vec3 physics = texture(uPhysics, vec2(vTexCoord.x, vTexCoord.y)).xyz;
	float roughness = physics.g;
	float metallic = physics.b;
#else
	#if defined(float_uRoughness)
		float roughness = uRoughness;
	#else
		float roughness = 1.0;
	#endif
	#if defined(float_uMetallic)
		float metallic = uMetallic;
	#else
		float metallic = 1.0;
	#endif
#endif
	float occlusion = inputs.ao;

#ifdef sampler2D_uShadowMap
	vec3 surfacePos = inputs.shadowPos*0.5+0.5;
	float casterDepth = texture(uShadowMap, surfacePos.xy).x;
	float shadowHardness = 1.0;
	float shadowEffect = 1.0-shadowHardness*max(0.0, dot(-uLightDir, inputs.normal));
	float shadowMask = (casterDepth < min(1.0, surfacePos.z)) ? shadowEffect : 1.0;
	if((surfacePos.x < 0.0) || (surfacePos.x > 1.0)
		|| (surfacePos.y < 0.0) || (surfacePos.y > 1.0))
	{
		shadowMask = 1.0;
	}
#else
	float shadowMask = 1.0;
#endif

#ifdef Furnace
	occlusion = 1.0;
	shadowMask = 1.0;
#endif
	if(baseColor.a < 0.5)
		discard;

	metallic = 0.1;
	baseColor.xyz = vec3(1.0,1.0,1.0);
	//float c0 = sin(vtxShadowPos.x*20) * sin(vtxShadowPos.y*40) * sin(vtxShadowPos.z*40);
	//baseColor.xyz *= vec3((tan(c0*10)*c0 > 0.05)? 0.0 : 1.0);
	roughness = 0.01;
	const float dielectricF0 = 0.04;
	vec3 F0 = mix(vec3(dielectricF0), baseColor.xyz, metallic);
	vec3 albedo = baseColor.xyz * (1-metallic);
#ifdef sampler2D_uEnvironment
	vec3 color = ibl(F0, inputs.normal, inputs.eye, albedo, roughness, occlusion, shadowMask, inputs.ndv);
#else
	vec3 color = baseColor.xyz;
#endif
	//color = vec3(shadowMask);
	return vec4(color, baseColor.a);
}

#endif // PXL_SHADER