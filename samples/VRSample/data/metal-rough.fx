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
#endif
#ifdef sampler2D_uIrradiance
layout(location = 8) uniform sampler2D uIrradiance;
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

//---------------------------------------------------------------------------------------
vec3 specularPBR(
	ShadeInput inputs,
	vec3 specColor,
	float roughness,
	float metallic,
	float ndl
	)
{
	vec3 halfV = normalize(inputs.eye + uLightDir);
	float ndh = max(1e-8,dot(inputs.normal, halfV));

	float F = fresnelSchlick(inputs.ndv);
	float NDF = GGX_NDF(ndh, roughness);
	float G   = GeometrySmithGGX(vec2(inputs.ndv, ndl), roughness);
	float specBRDF = F*G*NDF / 4.0;
	
	return mix(specColor, vec3(1.0), specBRDF);
}

//---------------------------------------------------------------------------------------
vec3 directLightPBR(
	ShadeInput inputs,
	vec3 diffColor,
	vec3 specColor,
	float roughness,
	float metallic
	)
{
	float ndl = max(0.0,dot(inputs.normal,uLightDir));
	vec3 diffuse = diffColor * ((1.0-metallic) * INV_PI);
	vec3 specular = specularPBR(inputs, specColor, roughness, metallic, ndl);
	
	//return (specular) * ndl * uLightColor;
	return (diffuse + specular) * ndl * uLightColor;
}

//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSpherical(vec3 v)
{
	vec2 uv = vec2(atan(v.y, -v.x), asin(-v.z));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float distortion(vec3 Wn)
{
  // Computes the inverse of the solid angle of the (differential) pixel in
  // the cube map pointed at by Wn
  float sinT = sqrt(1.0-Wn.y*Wn.y);
  return sinT;
}

#ifdef ANDROID
int nbSamples = 4;
vec2 fibonacci2D[nbSamples] = {
	vec2(0.7853981633974483, 0.6180300000000001),
	vec2(2.356194490192345, 0.23606000000000016),
	vec2(3.9269908169872414, 0.8540900000000002),
	vec2(5.497787143782138, 0.4721200000000003)
}
#else
const int nbSamples = 8;
// x component premultiplied by 2*pi
const vec2 fibonacci2D[nbSamples] = vec2[](
	vec2(0.39269908169872414, 0.6180300000000001),
	vec2(1.1780972450961724, 0.23606000000000016),
	vec2(1.9634954084936207, 0.8540900000000002),
	vec2(2.748893571891069, 0.4721200000000003),
	vec2(3.5342917352885173, 0.09015000000000128),
	vec2(4.319689898685965, 0.7081800000000005),
	vec2(5.105088062083414, 0.32620999999999967),
	vec2(5.890486225480862, 0.9442400000000006)
);
#endif
float computeLOD(vec3 Ln, float p)
{
//#if __VERSION__ >= 430
//	float maxLod = textureQueryLevels(uEnvironment);
//#else
	float maxLod = 8.0;
//#endif
	return max(0.0, (maxLod-1.5) - 0.5 * log2(float(nbSamples) * p * distortion(Ln)));
}

//---------------------------------------------------------------------------------------
struct LocalVectors
{
	vec3 eye;
	vec3 tangent;
	vec3 bitangent;
	vec3 normal;
};

vec3 gradient3d(vec3 dir)
{
#ifdef Furnace
	return vec3(1.0);
#else
	float f = 0.5f + 0.5f * dir.y;
	return vec3(0.5f, 0.7f, 1.f)*f + (1-f);
#endif
}

//---------------------------------------------------------------------------------------
vec3 specularIBL(
	LocalVectors vectors,
	vec3 specColor,
	float roughness,
	float occlusion,
	float ndv)
{
	vec3 radiance = vec3(0.0);
	float glossiness = 1.0 - roughness;

	for(int i=0; i<nbSamples; ++i)
	{
		vec3 Hn = importanceSampleGGX(
			fibonacci2D[i], vectors.tangent, vectors.bitangent, vectors.normal, roughness);
		vec3 Ln = -reflect(vectors.eye,Hn);

		float ndl = dot(vectors.normal, Ln);
		ndl = max( 1e-8, ndl );
		float vdh = max(1e-8, dot(vectors.eye, Hn));
		float ndh = max(1e-8, dot(vectors.normal, Hn));
		float lodS = roughness < 0.01 ? 0.0 : computeLOD(Ln, probabilityGGX(ndh, vdh, roughness));
#ifdef sampler2D_uEnvironment
		vec3 env = textureLod(uEnvironment, sampleSpherical(Ln), lodS ).xyz;
#else
		vec3 env = gradient3d(Ln);
#endif
		radiance += env * cook_torrance_contrib(vdh, ndh, ndl, ndv, specColor, roughness);
	}
	// Remove occlusions on shiny reflections
	radiance *= mix(occlusion, 1.0, glossiness * glossiness) / float(nbSamples);

	return radiance;
}

//---------------------------------------------------------------------------------------
vec3 diffuseIBL(ShadeInput inputs, vec3 diffColor, float occlusion)
{
#ifdef sampler2D_uIrradiance
	return diffColor * textureLod(uIrradiance, sampleSpherical(inputs.normal), 0.0).xyz * occlusion;
#else
	return diffColor * gradient3d(inputs.normal) * occlusion;
	//return inputs.normal;
#endif
	//return textureLod(uIrradiance, sampleSpherical(inputs.normal), 0).xyz * occlusion;
}

//---------------------------------------------------------------------------------------
vec3 indirectLightPBR(
	ShadeInput inputs,
	vec3 diffColor,
	vec3 specColor,
	float roughness,
	float occlusion
	//float shadow
	)
{
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
	vec3 specular = specularIBL(vectors, specColor, roughness, occlusion, inputs.ndv);// * shadow;
	vec3 diffuse = diffuseIBL(inputs, diffColor, occlusion);
	//vec3 diffuse = diffuseIBL(inputs, vec3(1.0), occlusion);
	
	//return specular + diffuse;
	return diffuse;
}

vec4 getBaseColor()
{
#if defined(sampler2D_uBaseColorMap) && defined(vec4_uBaseColor)
	vec4 baseColorTex = texture(uBaseColorMap, vTexCoord);
	vec4 baseColor = baseColorTex*uBaseColor;
#else
	#if defined(sampler2D_uBaseColorMap)
		vec4 baseColor = texture(uBaseColorMap, vTexCoord);
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
	float roughness = max(0.01, physics.g);
	float metallic = physics.b;
	float occlusion = physics.r;
#else
	#if defined(float_uRoughness)
	float roughness = uRoughness;
	#else
	float roughness = 0.8;
	#endif
	#if defined(float_uMetallic)
	float metallic = uMetallic;
	#else
	float metallic = 0.1;
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
	baseColor = vec4(1.0);
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
	float ggx = INV_PI * a2 / (ggxDen*ggxDen);

	float ndl = max(0.0, -dot(uLightDir, inputs.normal));

	// GGX Geometry schlick
	float g2denA = 2*ndl*inputs.ndv;
	float g2denB = ndl+inputs.ndv;
	float g2 = max(0.01, 2*mix(g2denA, g2denB, alpha));

	float sbrdf = ggx / g2; // Pure mirror brdf
	vec3 specular = Fs * sbrdf; // complete specular brdf

	// Single bounce diffuse
	vec3 albedo = baseColor.xyz * (1-metallic);
	//vec3 Kd = (vec3(1.0) - fresnel(ndl, F0)) * INV_PI; // Approximate fresnel with reflectance of perfectly smooth surface
	// Single bounce diffuse with analytical solution
	vec3 Kd = (1-Fs)*1.05*(1-sphg) * INV_PI;
	vec3 diffuse = Kd * albedo;
	// Multiple bounce diffuse (WIP)
	// float facing = 0.5 - 0.5*dot(uLightDir, inputs.eye);
	// float roughTerm = facing*(0.9-0.4*facing)*(0.5+ndh)/ndh;
	// float smoothTerm = 1.05*(1-pow(1-ndl, 5))*(1-pow(1-inputs.ndv,5));
	// float single = INV_PI * mix(smoothTerm, roughTerm, alpha);
	// float multi = 0.1159*alpha;
	// vec3 diffuse = albedo * (single + albedo * multi);

	vec3 indirect = indirectLightPBR(
		inputs,
		albedo,
		F0,
		roughness,
		occlusion
		);

	// Complete brdf
	vec3 direct = 4*uLightColor * (specular + diffuse) * ndl;

#if defined(sampler2D_uEmissive) && !defined(Furnace)
	vec3 emissive = texture(uEmissive, vTexCoord).xyz;
	vec3 color = indirect + shadowMask * direct + emissive;
#else
	vec3 color = indirect + shadowMask * direct;
#endif
	// color = 0.5+0.5*inputs.normal;
	return vec4(color, baseColor.a);
}

#endif // PXL_SHADER