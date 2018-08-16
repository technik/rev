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

// Material
#ifdef sampler2D_uEnvironment
layout(location = 7) uniform sampler2D uEnvironment;
layout(location = 8) uniform sampler2D uEnvBRDF;
layout(location = 18) uniform float numEnvLevels;
#endif
#ifdef vec4_uBaseColor
layout(location = 14) uniform vec4 uBaseColor;
#endif

#ifdef sampler2D_uBaseColorMap
layout(location = 11) uniform sampler2D uBaseColorMap;
#endif
layout(location = 12) uniform sampler2D uPhysics;
layout(location = 13) uniform sampler2D uEmissive;
#ifdef float_uRoughness
layout(location = 15) uniform float uRoughness;
#endif
#ifdef float_uMetallic
layout(location = 16) uniform float uMetallic;
#endif
#ifdef sampler2D_uFms
layout(location = 17) uniform sampler2D uFms;
#endif

//---------------------------------------------------------------------------------------
struct LocalVectors
{
	vec3 eye;
	vec3 tangent;
	vec3 bitangent;
	vec3 normal;
};

//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.1591, -0.3183);
vec2 sampleSpherical(vec3 v)
{
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += vec2(0.0, 0.5);
    return uv;
}

//---------------------------------------------------------------------------------------
vec3 specularIBL(
	ShadeInput inputs,
	LocalVectors vectors,
	vec3 specColor,
	float roughness,
	float occlusion,
	float ndv)
{
#if defined(sampler2D_uEnvironment) && !defined(Furnace)
	float lodLevel = roughness*roughness * numEnvLevels;
	vec3 samplerDir = reflect(-inputs.eye, inputs.normal);
	vec3 radiance = textureLod(uEnvironment, sampleSpherical(samplerDir), lodLevel).xyz;
	vec2 envBRDF = textureLod(uEnvBRDF, vec2(min(0.99,ndv), 1.0-roughness), 0).xy;
	// Multiple scattering 

	vec3 Favg = (13+29*specColor)/42;
	float fms = textureLod(uFms, vec2(inputs.ndv, roughness), 0).x;
	//fms = 0.0;
	vec3 irradiance = textureLod(uEnvironment, sampleSpherical(inputs.normal), int(numEnvLevels)).xyz;

	return radiance * (specColor * envBRDF.x + envBRDF.y) + Favg * fms * irradiance * occlusion;
#else
	return specColor * envBRDF.x + envBRDF.y;
#endif
}

//---------------------------------------------------------------------------------------
vec3 diffuseIBL(ShadeInput inputs, vec3 diffColor, float occlusion)
{
#if defined(sampler2D_uIrradiance) && !defined(Furnace)
	return diffColor * textureLod(uEnvironment, sampleSpherical(inputs.normal), int(numEnvLevels)).xyz * occlusion;
#else
	// Furnace test environment
	return vec3(1.0) * occlusion;
#endif
}

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

//---------------------------------------------------------------------------------------
vec4 shadeSurface(ShadeInput inputs)
{
	vec4 baseColor = getBaseColor();

#ifdef sampler2D_uPhysics
	vec3 physics = texture(uPhysics, vTexCoord).xyz;
	float roughness = physics.g;
	float metallic = physics.b;
	float occlusion = 1.0;//physics.r;
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
	float occlusion = 1.0;
#endif

#ifdef sampler2D_uShadowMap
	float shadowDepth = texture(uShadowMap, inputs.shadowPos.xy*0.5+0.5).x;
	float surfaceDepth = 0.5 + 0.5 * inputs.shadowPos.z;
	//float shadowHardness = 0.7;
	//float shadowEffect = 1.0-shadowHardness*max(0.0, dot(-uLightDir, inputs.normal));
	//float shadowEffect = 0.0;//-shadowHardness*max(0.0, dot(-uLightDir, inputs.normal));
	//float shadowMask = shadowDepth;// > surfaceDepth) ? 0.0 : 1.0;
	float shadowMask = (shadowDepth > surfaceDepth) ? 0.0 : 1.0;
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
	//directSpecular = vec3(0.0);

	// Single bounce diffuse
	vec3 albedo = baseColor.xyz * (1-metallic);
	float ff = 1.05 * INV_PI * (1-pow(1-inputs.ndv,5)); // Fresnel radiance correction
#if defined(sampler2D_uEmissive) && !defined(Furnace)
	vec3 emissive = texture(uEmissive, vTexCoord).xyz;
	vec3 directDiffuse = (uLightColor * albedo +emissive) * ff;
#else
	vec3 directDiffuse = uLightColor * albedo * ff;
#endif

	// -------- Indirect 
	LocalVectors vectors;
	vectors.eye = inputs.eye;
#ifdef VTX_TANGENT_SPACE
	vectors.tangent = inputs.tangent;
	vectors.bitangent = inputs.bitangent;
#else
	// Construct a local orthonormal base
	vec3 tangent = inputs.normal.zxy;
	tangent = tangent - dot(tangent, inputs.normal) * inputs.normal;
	vectors.tangent = normalize(tangent);
	vectors.bitangent = cross(inputs.normal, vectors.tangent);
#endif
	vectors.normal = inputs.normal;
	vec3 indirectSpecular = specularIBL(inputs, vectors, F0, max(0.001,roughness), occlusion, inputs.ndv);
	vec3 indirectDiffuse = diffuseIBL(inputs, albedo, occlusion);

	vec3 indirect = indirectSpecular + indirectDiffuse;

#ifdef Furnace
	ndl = 0.0;
#endif
	//vec3 diffuse = directDiffuse * shadowMask * ndl + indirectDiffuse;
	vec3 diffuse = indirectDiffuse;
	//vec3 specular = (0.0*uLightColor * directSpecular * ndl + indirectSpecular);
	vec3 specular = indirectSpecular;

	vec3 color = diffuse + specular;
	//return vec4(vec3(roughness), baseColor.a);
	return vec4(color, baseColor.a);
}

#endif // PXL_SHADER