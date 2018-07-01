#ifdef PXL_SHADER

#include "pbr.fx"

// Material
layout(location = 11) uniform sampler2D uAlbedo;

vec3 shadeSurface(ShadeInput inputs)
{
	float roughness = 0.5;
	float metallic = 0.1;
	vec3 F0 = vec3(0.04);
	vec3 albedo = texture(uAlbedo, vTexCoord).xyz;
	F0      = mix(F0, albedo, metallic);
	vec3 halfV = normalize(inputs.eye + uLightDir);
	vec3 F  = fresnel(inputs.ndv, F0);

	float ndh = max(0.0, dot(halfV,inputs.normal));
	
	float NDF = GGX_NDF(ndh, roughness);
	float ndl = max(1e-8,dot(uLightDir,inputs.normal));
	float G   = GeometrySmith(vec2(inputs.ndv, ndl), roughness);
	
	vec3 nominator    = NDF * G * F;
	float denominator = 4.0 * inputs.ndv * ndl;
	vec3 specular     = nominator / max(denominator, 0.001);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	  
	kD *= 1.0 - metallic;
	
	return (kD * albedo / PI + specular) * uLightColor * ndl;
}

#endif // PXL_SHADER