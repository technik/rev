#ifdef PXL_SHADER

#include "pbr.fx"

// Material
layout(location = 7) uniform sampler2D uEnvironment;
layout(location = 8) uniform sampler2D uIrradiance;
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
		vec3 env = textureLod(uEnvironment, sampleSpherical(Ln), lodS ).xyz;
		radiance += env * cook_torrance_contrib(vdh, ndh, ndl, ndv, specColor, roughness);
	}
	// Remove occlusions on shiny reflections
	radiance *= mix(occlusion, 1.0, glossiness * glossiness) / float(nbSamples);

	return radiance;
}

//---------------------------------------------------------------------------------------
vec3 diffuseIBL(ShadeInput inputs, vec3 diffColor, float occlusion)
{
	return diffColor * textureLod(uIrradiance, sampleSpherical(inputs.normal), 0.0).xyz * occlusion;
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
	/*float shadowImportance = max(0.0,dot(inputs.normal, uLightDir));
	shadowImportance = sqrt(shadowImportance);
	//shadowImportance = shadowImportance*shadowImportance;
	float shadowMask = mix(1.0, shadow, shadowImportance);*/
	LocalVectors vectors;
	vectors.eye = inputs.eye;
#ifdef sampler2D_uNormalMap
	vectors.tangent = inputs.tangent;
	vectors.bitangent = inputs.bitangent;
#endif
	vectors.normal = inputs.normal;
	vec3 specular = specularIBL(vectors, specColor, roughness, occlusion, inputs.ndv);
	vec3 diffuse = diffuseIBL(inputs, diffColor, occlusion);
	
	return specular + diffuse;
	//return (1.0-0.00001*shadowMask)* ( diffuse);
	//return diffuse;
	//return specular;
	//return shadowMask * diffuse;
	//return vec3(shadow);
	//return vec3(shadowMask);
}

//---------------------------------------------------------------------------------------
vec3 shadeSurface(ShadeInput inputs)
{
#if defined(sampler2D_uBaseColorMap) && defined(vec4_uBaseColor)
	vec4 baseColorTex = texture(uBaseColorMap, vTexCoord);
	vec3 baseColor = (baseColorTex*uBaseColor).xyz;
#else
	#if defined(sampler2D_uBaseColorMap)
		vec3 baseColor = texture(uBaseColorMap, vTexCoord).xyz;
	#else
		#if defined(vec4_uBaseColor)
			vec3 baseColor = uBaseColor.xyz;
		#else
			vec3 baseColor = vec3(1.0);
		#endif
	#endif
#endif

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

	//vec4 shadowSpacePos = 0.5 + 0.5*(uMs2Shadow * vec4(vtxWsPos, 1.0));
	//vec4 shadowSpacePos = 0.5 + 0.5*(uMs2Shadow * vec4(vtxWsPos, 1.0));
	//vec4 shadowSpacePos = vec4(vtxWsPos, 1.0);
	//float sampledDepth = texture(uShadowMap, shadowSpacePos.xy).x;
	//float curDepth = shadowSpacePos.z;
	//float shadow = 1.0;
	/*if(shadowSpacePos.x >= 0.0 && shadowSpacePos.x <= 1.0 &&
	 shadowSpacePos.y >= 0.0 && shadowSpacePos.y <= 1.0 &&
	 shadowSpacePos.z >= 0.0 && shadowSpacePos.z <= 1.0 &&
	 shadowSpacePos.z > sampledDepth)
		shadow = 0.0;*/
	
	vec3 specColor = mix(vec3(0.04), baseColor, metallic);
	vec3 diffColor = baseColor*(1.0-metallic);

	/*vec3 directLight = directLightPBR(
		inputs,
		diffColor,
		specColor,
		roughness,
		metallic);*/
	vec3 indirectLight = indirectLightPBR(
		inputs,
		diffColor,
		specColor,
		roughness,
		occlusion);
	//return 0.5+0.5*inputs.normal;
	//return directLight + emissive;
	//return shadowSpacePos.xyz;
	//return vec3(shadowSpacePos.xy, sampledDepth);
	//return indirectLight;
	//return vec3(roughness);//indirectLight;
	//return indirectLight + emissive;
	//return emissive;
	//return physics;
	//return baseColor;
	//return directLight + indirectLight + emissive;
	
#ifdef sampler2D_uEmissive
	vec3 emissive = texture(uEmissive, vTexCoord).xyz;
	return indirectLight + emissive;
#else
	return indirectLight;
#endif
	//return directLight + emissive;
}

#endif // PXL_SHADER