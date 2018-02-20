#ifdef PXL_SHADER

//------------------------------------------------------------------------------
float GGX(float NdH, float a)
{
    float a2     = a*a;
    float NdH2 = NdH*NdH;
	
    float nom    = a2;
    float denom  = (NdH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

//------------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

//------------------------------------------------------------------------------
float GeometrySmith(float NdV, float NdL, float roughness)
{
    float ggx2  = GeometrySchlickGGX(NdV, roughness);
    float ggx1  = GeometrySchlickGGX(NdL, roughness);
	
    return ggx1 * ggx2;
}

//------------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	float negCos = 1.0 - cosTheta;
	float pow5 = negCos * negCos;
	pow5 = pow5 * pow5 * negCos;
    return F0 + (1.0 - F0) * pow5;
}

// Material
layout(location = 5) uniform sampler2D uAlbedo;
layout(location = 6) uniform sampler2D uPhysics;
layout(location = 7) uniform sampler2D uEmissive;
layout(location = 8) uniform sampler2D uAO;
layout(location = 9) uniform sampler2D uNormalMap;

//---------------------------------------------------------------------------------------
vec3 directLightPBR(
	ShadeInput inputs,
	vec3 albedo,
	float roughness,
	float metallic
	)
{
	vec3 F0 = mix(vec3(0.04), albedo, metallic);
	vec3 F  = fresnelSchlick(inputs.ndv, F0);
	
	float NDF = GGX(inputs.ndh, roughness);
	float G   = GeometrySmith(inputs.ndv, inputs.ndl, roughness);
	
	vec3 nominator    = NDF * G * F;
	float denominator = 4.0 * inputs.ndv * inputs.ndl;
	vec3 specular     = nominator / max(denominator, 0.001);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	  
	kD *= 1.0 - metallic;
	
	return (kD * albedo / PI + specular) * lightColor * inputs.ndl;
}

//---------------------------------------------------------------------------------------
vec3 indirectLightPBR(
	vec3 albedo,
	float oclussion
	)
{
	return 0.1 * albedo * oclussion;
}

//---------------------------------------------------------------------------------------
vec3 shadeSurface(ShadeInput inputs)
{
	vec3 albedo = texture(uAlbedo, vTexCoord).xyz;
	albedo = pow(albedo, 1.0/vec3(2.2,2.2,2.2));
	vec2 physics = texture(uPhysics, vTexCoord).xy;
	float roughness = physics.r*physics.r;
	float metallic = physics.g;
	float oclussion = texture(uAO, vTexCoord).x;
	
	
	
	vec3 emissive = texture(uEmissive, vTexCoord).xyz;
	
	vec3 directLight = directLightPBR(
		inputs,
		albedo,
		roughness,
		metallic);
	vec3 indirectLight = indirectLightPBR(albedo, oclussion);
	
	return directLight
		+ indirectLight
		+ emissive;
}

#endif // PXL_SHADER