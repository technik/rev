#ifndef IBL_FX
#define IBL_FX

layout(location = 6) uniform float numEnvLevels;

// IBL
//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.159154943, 0.31830988);
vec2 sampleSpherical(vec3 v)
{
  vec2 uv = vec2(atan(-v.x, v.z), asin(-v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//---------------------------------------------------------------------------------------
vec3 getIrradiance(vec3 dir)
{
#if defined(sampler2D_uEnvironment) && !defined(Furnace)
  return textureLod(uEnvironment, sampleSpherical(dir), numEnvLevels).xyz;
#else
  return vec3(1.0);
#endif
}

//---------------------------------------------------------------------------------------
vec3 getRadiance(vec3 dir, float lodLevel)
{
#if defined(sampler2D_uEnvironment) && !defined(Furnace)
  return textureLod(uEnvironment, sampleSpherical(dir), lodLevel).xyz;
#else
  return vec3(1.0);
#endif
}

#ifdef sampler2D_uEnvironment
//---------------------------------------------------------------------------------------
vec3 ibl(
  vec3 F0,
  vec3 normal,
  vec3 eye,
  vec3 albedo,
  vec3 lightDir,
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

	float lodLevel = roughness * numEnvLevels;
	vec3 reflDir = reflect(-eye, normal);
	vec3 radiance = getRadiance(reflDir, lodLevel); // Prefiltered radiance
	vec3 irradiance = getIrradiance(normal); // Cosine-weighted irradiance

	// Multiple scattering
	float Ess = f_ab.x + f_ab.y;
	float Ems = 1-Ess;
	vec3 Favg = F0 + (1-F0)*0.0476190476;
	vec3 Fms = FssEss*Favg/(1-(1-Ess)*Favg);

	// Dielectrics
	vec3 Edss = 1 - (FssEss + Fms * Ems);
	vec3 kD = albedo * Edss;

    // Shadow contributions
    float sdwD = max(0.0, -dot(lightDir,normal));
    float sdwS = max(0.0, -dot(lightDir,reflDir));
	// Composition
	vec3 occDiff = vec3(occlusion);
    
	// Remapping for multiple bounces
	{
		vec3 a = 2.0404 * albedo - 0.3324;
		vec3 b = -4.7951 * albedo + 0.6417;
		vec3 c = 2.7552 * albedo + 0.6903;

		occDiff = max(vec3(occlusion), ((occlusion*a+b)*occlusion+c)*occlusion);
	}

    return (sdwS*(shadow-1)+1)*FssEss * radiance
	 + (sdwD*(shadow-1)+1)*(Fms*Ems+kD) * irradiance*occDiff;
}
#endif // sampler2D_uEnvironment

#endif