#ifndef IBL_FX
#define IBL_FX
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
	//vec3 Favg = F0 + (1-F0)*0.14959965; // Pi/21
	//if(normal.x > 0)
	vec3 Favg = F0 + (1-F0)*0.0476190476; // Pi/21
	vec3 Fms = FssEss*Favg/(1-(1-Ess)*Favg);

	// Dielectrics
	vec3 Edss = 1 - (FssEss + Fms * Ems);
	vec3 kD = albedo * Edss;

	// Composition
	return FssEss * radiance + shadow*(Fms*Ems+kD) * irradiance;
}
#endif // sampler2D_uEnvironment

#endif