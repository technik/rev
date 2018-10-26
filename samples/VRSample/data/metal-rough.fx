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

layout(location = 12) uniform sampler2D uPhysics;
layout(location = 13) uniform sampler2D uEmissive;
#ifdef float_uRoughness
layout(location = 15) uniform float uRoughness;
#endif
#ifdef float_uMetallic
layout(location = 16) uniform float uMetallic;
#endif

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

#ifdef sampler2D_uEnvironment
//---------------------------------------------------------------------------------------
vec3 specularIBL(
  vec3 irradiance,
  vec3 F0,
  vec3 normal,
  vec3 eye,
  float roughness,
  float occlusion,
  vec3 Fms,
  vec3 fms,
  float ndv)
{
  // Single scattering
	#if 10
		float lodLevel = roughness * (1.3-0.3*roughness) * numEnvLevels;
	#else
		float lodLevel = roughness * roughness * numEnvLevels;
	#endif
	vec3 samplerDir = reflect(-eye, normal);
	vec3 radiance = getRadiance(samplerDir, lodLevel);

	return radiance * (F0 * fms.x + fms.y) * occlusion + Fms *fms.z * irradiance;
}

//---------------------------------------------------------------------------------------
vec3 diffuseIBL(vec3 kD, vec3 irradiance, vec3 diffColor, float occlusion)
{
	return kD * diffColor * irradiance * occlusion;
}

//---------------------------------------------------------------------------------------
vec3 ibl(
  vec3 F0,
  vec3 normal,
  vec3 eye,
  vec3 albedo,
  float roughness,
  float occlusion,
  float ndv
  )
{
	// Common code for single and multiple scattering
	vec3 Fr = max(vec3(1.0 - roughness), F0) - F0; // Roughness dependent fresnel
	vec3 kS = F0 + Fr * pow(1.0-ndv, 5.0);

	vec2 f_ab = textureLod(uEnvBRDF, vec2(ndv, roughness), 0).xy;
	vec3 Fssfss = (kS * f_ab.x + f_ab.y);

	float lodLevel = roughness * (1.3-0.3*roughness) * numEnvLevels;
	vec3 samplerDir = reflect(-eye, normal);
	vec3 radiance = getRadiance(samplerDir, lodLevel); // Prefiltered radiance
	vec3 irradiance = getIrradiance(normal); // Cosine-weighted irradiance

	// Multiple scattering
	float fms = 1-(f_ab.x+f_ab.y); // MS energy
	vec3 Favg = F0 + Fr/3; // Average fresnel
	vec3 Fms = Fssfss*Favg/(1-Favg*fms);

	// Dielectrics
	vec3 kD = 1.0 - Fssfss - Fms * fms;

	// Composition
	return Fssfss * radiance + (Fms * fms + kD*albedo) * irradiance;
}
#endif // sampler2D_uEnvironment

//---------------------------------------------------------------------------------------
vec4 shadeSurface(ShadeInput inputs)
{
	vec4 baseColor = getBaseColor();

#ifdef sampler2D_uPhysics
	vec3 physics = texture(uPhysics, vTexCoord).xyz;
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
	float occlusion = 1.0;

#ifdef sampler2D_uShadowMap
	float shadowDepth = texture(uShadowMap, inputs.shadowPos.xy*0.5+0.5).x;
	float surfaceDepth = 0.5 + 0.5 * inputs.shadowPos.z;
	float shadowHardness = 0.7;
	float shadowEffect = 1.0-shadowHardness*max(0.0, dot(uLightDir, inputs.normal));
	//float shadowEffect = 0.0;//-shadowHardness*max(0.0, dot(-uLightDir, inputs.normal));
	float shadowMask = (shadowDepth > surfaceDepth) ? shadowEffect : 1.0;
#else
	float shadowMask = 1.0;
#endif

#ifdef Furnace
	occlusion = 1.0;
	shadowMask = 1.0;
#endif
	if(baseColor.a < 0.5)
		discard;

	const float dielectricF0 = 0.04;
	vec3 F0 = mix(vec3(dielectricF0), baseColor.xyz, metallic);

	// specular brdf (geometric terms)
	float alpha = roughness * roughness;
	float a2 = alpha * alpha; // No need to clamp at 0 bc r >= 0.01

	// Analytical directional light
	vec3 h = normalize(inputs.eye - uLightDir);
	float hdv = max(0.0, dot(h, inputs.eye));
	// Spherical gaussian approx of fresnel schlick
	float sphg = exp2((-5.55473*hdv - 6.98316) * hdv);
	//float oneMinusSphg = 1-sphg;
	vec3 Fs =  F0 * (1.0 - sphg) + sphg; // Same lerp, change one vector op by one scalar op

	// GGX NDF
	float ndh = max(1e-8, dot(inputs.normal, h));
	float ndh2 = ndh*ndh;
	float ggxDen = (ndh2 * (a2-1) + 1);
	float ggx = a2 * INV_PI / (ggxDen*ggxDen);

	float ndl = max(0.0, -dot(uLightDir, inputs.normal));

	// GGX Geometry schlick
	float g2denA = 2*ndl*inputs.ndv;
	float g2denB = ndl+inputs.ndv;
	float g2 = max(0.01, 2*mix(g2denA, g2denB, alpha)); // Denominator from Hammon GDC 17's PBR diffuse lighting

	float sbrdf = ggx / g2; // Pure mirror brdf
	float emsV = textureLod(uFms, vec2(inputs.ndv, roughness), 0).x;
	float emsL = textureLod(uFms, vec2(ndl, roughness), 0).x;
	vec3 fmsA = vec3(0.592665, -1.47034, 1.47196);
	float r3 = alpha*roughness;
	float OneMinEavg = (roughness == 0) ? 0.0 : fmsA.x*r3 / (1+fmsA.y*roughness+fmsA.z*alpha);
	float Eavg = 1-OneMinEavg;
	float fms = emsV*emsL/max(0.01, OneMinEavg);
	fms = 0.0;
	vec3 Favg = (13+8*F0)/42;
	vec3 Fms = Favg*Favg*Eavg / (1-Favg*OneMinEavg);
	vec3 directSpecular = Fs * sbrdf;// + Fms * fms; // complete specular brdf

	// Single bounce diffuse
	vec3 albedo = baseColor.xyz * (1-metallic);
	float ff = 1.05 * INV_PI * (1-pow(1-inputs.ndv,5)); // Fresnel radiance correction
#if defined(sampler2D_uEmissive) && !defined(Furnace)
	vec3 emissive = texture(uEmissive, vTexCoord).xyz;
	vec3 directDiffuse = (uLightColor * albedo +emissive) * ff;
#else
	vec3 directDiffuse = uLightColor * albedo * ff;
#endif

#ifdef sampler2D_uEnvironment
	vec3 color = ibl(F0, inputs.normal, inputs.eye, albedo, roughness, occlusion*shadowMask, inputs.ndv);
#else
	vec3 color = albedo;
#endif
	color = color;
	return vec4(color, baseColor.a);
}

#endif // PXL_SHADER