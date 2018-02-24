#ifdef PXL_SHADER

//------------------------------------------------------------------------------
float GGX_NDF(float ndh, float a)
{
    float a2     = a*a;
    float ndh2 = ndh*ndh;
	
    float denom  = ndh2 * (a2 - 1.0) + 1.0;
    denom        = PI * denom * denom;
	
    return a2 / denom;
}

//------------------------------------------------------------------------------
// Actually: GGX divided by ndv
// SIMD optimized version
vec2 GeometrySchlickGGX(vec2 ndv_ndl, float roughness)
{
	float k = roughness/2.0;
    vec2 denom = ndv_ndl*(1.0-k) + k;
	
    return 1.0 / denom;
}

//------------------------------------------------------------------------------
// Actually: Smith GGX divided by (ndv * ndl)
// SIMD optimized version
float GeometrySmithGGX(vec2 ndv_ndl, float roughness)
{
    vec2 ggx  = GeometrySchlickGGX(ndv_ndl, roughness);
    return ggx.x * ggx.y;
}

#define SPHERICAL_GAUSIAN_SCHLICK
//------------------------------------------------------------------------------
float fresnelSchlick(float ndv)
{
#ifdef SPHERICAL_GAUSIAN_SCHLICK
	// Schlick with Spherical Gaussian approximation
	// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
	return pow(2.0, (-5.55473*ndv - 6.98316) * ndv);
#else // !SPHERICAL_GAUSIAN_SCHLICK
	//return pow(ndl, 5.0);
	float n = 1.0-ndv;
	float n2 = n*n;
	return n2*n2*n;
#endif // !SPHERICAL_GAUSIAN_SCHLICK
}

// Material
layout(location = 7) uniform sampler2D uEnvironment;
layout(location = 8) uniform sampler2D uIrradiance;
layout(location = 11) uniform sampler2D uAlbedo;
layout(location = 12) uniform sampler2D uPhysics;
layout(location = 13) uniform sampler2D uEmissive;
//layout(location = 14) uniform sampler2D uAO;

//---------------------------------------------------------------------------------------
vec3 diffusePBR(
	ShadeInput inputs,
	vec3 diffColor,
	float metallic
	)
{
	return diffColor * ((1.0-metallic) / PI);
}

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
	float specBRDF = F*G*NDF / 4;
	
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
	vec3 diffuse = diffusePBR(inputs, diffColor, metallic);
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

float normal_distrib(
  float ndh,
  float Roughness)
{
  // use GGX / Trowbridge-Reitz, same as Disney and Unreal 4
  // cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
  float alpha = Roughness * Roughness;
  float tmp = alpha / max(1e-8,(ndh*ndh*(alpha*alpha-1.0)+1.0));
  return tmp * tmp / PI;
}

vec3 fresnel(
  float vdh,
  vec3 F0)
{
  // Schlick with Spherical Gaussian approximation
  // cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
  float sphg = pow(2.0, (-5.55473*vdh - 6.98316) * vdh);
  return F0 + (vec3(1.0, 1.0, 1.0) - F0) * sphg;
}

float G1(
  float ndw, // w is either Ln or Vn
  float k)
{
  // One generic factor of the geometry function divided by ndw
  // NB : We should have k > 0
  return 1.0 / ( ndw*(1.0-k) +  k );
}

float visibility(
  float ndl,
  float ndv,
  float Roughness)
{
  // Schlick with Smith-like choice of k
  // cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
  // visibility is a Cook-Torrance geometry function divided by (n.l)*(n.v)
  float k = max(Roughness * Roughness * 0.5, 1e-5);
  return G1(ndl,k)*G1(ndv,k);
}

vec3 cook_torrance_contrib(
  float vdh,
  float ndh,
  float ndl,
  float ndv,
  vec3 Ks,
  float Roughness)
{
  // This is the contribution when using importance sampling with the GGX based
  // sample distribution. This means ct_contrib = ct_brdf / ggx_probability
  return fresnel(vdh,Ks) * (visibility(ndl,ndv,Roughness) * vdh * ndl / ndh );
}

vec3 importanceSampleGGX(vec2 Xi, vec3 T, vec3 B, vec3 N, float roughness)
{
  float a = roughness*roughness;
  float cosT = sqrt((1.0-Xi.y)/(1.0+(a*a-1.0)*Xi.y));
  float sinT = sqrt(1.0-cosT*cosT);
  float phi = 2.0*PI*Xi.x;
  return
    T * (sinT*cos(phi)) +
    B * (sinT*sin(phi)) +
    N *  cosT;
}

float probabilityGGX(float ndh, float vdh, float Roughness)
{
  return normal_distrib(ndh, Roughness) * ndh / (4.0*vdh);
}

float distortion(vec3 Wn)
{
  // Computes the inverse of the solid angle of the (differential) pixel in
  // the cube map pointed at by Wn
  float sinT = sqrt(1.0-Wn.y*Wn.y);
  return sinT;
}

int nbSamples = 32;
float computeLOD(vec3 Ln, float p)
{
	int maxLod = textureQueryLevels(uEnvironment);
	return max(0.0, (maxLod-1.5) - 0.5 * log2(float(nbSamples) * p * distortion(Ln)));
}

const float M_GOLDEN_RATIO = 1.618034;
float fibonacci1D(int i)
{
  return fract((float(i) + 1.0) * M_GOLDEN_RATIO);
}

//---------------------------------------------------------------------------------------
vec2 fibonacci2D(int i, int nSamples)
{
  return vec2(
    (float(i)+0.5) / float(nSamples),
    fibonacci1D(i)
  );
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
		vec2 Xi = fibonacci2D(i, nbSamples);
		vec3 Hn = importanceSampleGGX(
			Xi, vectors.tangent, vectors.bitangent, vectors.normal, roughness);
		vec3 Ln = -reflect(vectors.eye,Hn);

		float fade = 1.0;//horizonFading(dot(vectors.vertexNormal, Ln), horizonFade);

		float ndl = dot(vectors.normal, Ln);
		ndl = max( 1e-8, ndl );
		float vdh = max(1e-8, dot(vectors.eye, Hn));
		float ndh = max(1e-8, dot(vectors.normal, Hn));
		float lodS = roughness < 0.01 ? 0.0 : computeLOD(Ln, probabilityGGX(ndh, vdh, roughness));
		vec3 env = textureLod(uEnvironment, sampleSpherical(Ln), lodS ).xyz;
		radiance += fade * env *
			cook_torrance_contrib(vdh, ndh, ndl, ndv, specColor, roughness);
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
	float occlusion,
	float shadow
	)
{
	float shadowImportance = max(0.0,dot(inputs.normal, uLightDir));
	shadowImportance = sqrt(shadowImportance);
	//shadowImportance = shadowImportance*shadowImportance;
	float shadowMask = mix(1.0, shadow, shadowImportance);
	LocalVectors vectors;
	vectors.eye = inputs.eye;
	vectors.normal = inputs.normal;
	vectors.tangent = inputs.tangent;
	vectors.bitangent = inputs.bitangent;
	vec3 specular = specularIBL(vectors, specColor, roughness, occlusion, inputs.ndv);
	vec3 diffuse = diffuseIBL(inputs, diffColor, occlusion);
	
	return shadowMask* (specular +  diffuse);
	//return diffuse;
	//return specular;
	//return shadowMask * diffuse;
	//return vec3(shadow);
	//return vec3(shadowMask);
}

//---------------------------------------------------------------------------------------
vec3 shadeSurface(ShadeInput inputs)
{
	vec3 albedo = texture(uAlbedo, vTexCoord).xyz;
	vec3 physics = texture(uPhysics, vTexCoord).xyz;
	float roughness = max(0.01, physics.g);
	float metallic = physics.b;
	float occlusion = physics.r;

	//vec4 shadowSpacePos = 0.5 + 0.5*(uMs2Shadow * vec4(vtxWsPos, 1.0));
	vec4 shadowSpacePos = 0.5 + 0.5*(uMs2Shadow * vec4(vtxWsPos, 1.0));
	//vec4 shadowSpacePos = vec4(vtxWsPos, 1.0);
	float sampledDepth = texture(uShadowMap, shadowSpacePos.xy).x;
	float curDepth = shadowSpacePos.z;
	float shadow = 1.0;
	if(shadowSpacePos.x >= 0.0 && shadowSpacePos.x <= 1.0 &&
	 shadowSpacePos.y >= 0.0 && shadowSpacePos.y <= 1.0 &&
	 shadowSpacePos.z >= 0.0 && shadowSpacePos.z <= 1.0 &&
	 shadowSpacePos.z > sampledDepth)
		shadow = 0.0;
	
	vec3 emissive = texture(uEmissive, vTexCoord).xyz;
	
	vec3 specColor = mix(vec3(0.04), albedo, metallic);
	vec3 diffColor = albedo*(1.0-metallic);

	vec3 directLight = directLightPBR(
		inputs,
		diffColor,
		specColor,
		roughness,
		metallic);
	vec3 indirectLight = indirectLightPBR(
		inputs,
		diffColor,
		specColor,
		roughness,
		occlusion,
		shadow);
	//return directLight + emissive;
	//return shadowSpacePos.xyz;
	//return vec3(shadowSpacePos.xy, sampledDepth);
	//return indirectLight;
	//return vec3(shadow);//indirectLight;
	return indirectLight + emissive;
	//return directLight + indirectLight + emissive;
}

#endif // PXL_SHADER