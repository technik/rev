#ifndef IBL_FX
#define IBL_FX
// IBL
//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.159154943, -0.31830988);
vec2 sampleSpherical(vec3 v)
{
  vec2 uv = vec2(atan(-v.x, v.z), asin(v.y));
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
	float Ess = f_ab.x + f_ab.y;
	float Ems = 1-Ess;
	//Ems = 0.f;

	float lodLevel = roughness * (1.3-0.3*roughness) * numEnvLevels;
	vec3 samplerDir = reflect(normal, eye);
	vec3 radiance = getRadiance(samplerDir, lodLevel) * mix(occlusion * shadow,1,max(0.0,dot(normal,samplerDir))); // Prefiltered radiance
	vec3 irradiance = getIrradiance(normal) * occlusion * shadow; // Cosine-weighted irradiance

	// Multiple scattering
	vec3 Fms = vec3(0.0);//FssEss*FssEss/(Ess-FssEss*Ems);

	// Dielectrics
	vec3 Edss = 1 - (FssEss + Fms * Ems);
	vec3 kD = albedo * Edss / (1-albedo*(1-Edss));

	// Composition
	return (FssEss * radiance + (Fms * Ems + kD) * irradiance);
}
#endif // sampler2D_uEnvironment

#endif