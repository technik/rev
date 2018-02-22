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
layout(location = 5) uniform sampler2D uAlbedo;
layout(location = 6) uniform sampler2D uPhysics;
layout(location = 7) uniform sampler2D uEmissive;
layout(location = 8) uniform sampler2D uAO;
layout(location = 10) uniform sampler2D uEnvironment;
layout(location = 11) uniform sampler2D uIrradiance;

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
	float metallic
	)
{
	float F = fresnelSchlick(inputs.ndv);
	float NDF = GGX_NDF(inputs.ndh, roughness);
	float G   = GeometrySmithGGX(vec2(inputs.ndv, inputs.ndl), roughness);
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
	vec3 diffuse = diffusePBR(inputs, diffColor, metallic);
	vec3 specular = specularPBR(inputs, specColor, roughness, metallic);
	
	//return (diffuse + specular) * inputs.ndl * lightColor;
	return (diffuse + specular) * inputs.ndl;
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

//---------------------------------------------------------------------------------------
vec3 indirectLightPBR(
	ShadeInput inputs,
	vec3 diffColor,
	vec3 specColor,
	float oclussion
	)
{
	vec3 worldRefl = inputs.worldReflectDir;
	vec3 env = texture(uEnvironment, sampleSpherical(-worldRefl)).xyz;
	vec3 irradiance = texture(uIrradiance, sampleSpherical(inputs.worldNormal)).xyz;
	return (env * specColor + diffColor * irradiance) * oclussion;
}

//---------------------------------------------------------------------------------------
vec3 shadeSurface(ShadeInput inputs)
{
	vec3 albedo = texture(uAlbedo, vTexCoord).xyz;
	albedo = pow(albedo, 1.0/vec3(2.2,2.2,2.2));
	vec3 physics = texture(uPhysics, vTexCoord).xyz;
	physics = pow(physics, 1.0/vec3(2.2,2.2,2.2));
	float roughness = min(0.01,physics.g);
	float metallic = physics.b;
	float oclussion = physics.r;
	//float oclussion = pow(texture(uAO, vTexCoord).x, 1.0/2.2);
	
	vec3 emissive = texture(uEmissive, vTexCoord).xyz;
	
	vec3 specColor = mix(vec3(0.04), albedo, metallic);
	vec3 diffColor = vec3(1.0)-specColor;

	vec3 directLight = directLightPBR(
		inputs,
		diffColor,
		specColor,
		roughness,
		metallic);
	vec3 indirectLight = indirectLightPBR(inputs, diffColor, specColor, oclussion);
	
	//return vec3(oclussion);
	//retrun albedo;
	return directLight
		+ indirectLight
		+ emissive;
}

#endif // PXL_SHADER