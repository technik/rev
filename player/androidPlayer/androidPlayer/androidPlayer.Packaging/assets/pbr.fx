#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 0) uniform mat4 uWorldViewProjection;
layout(location = 2) uniform vec3 uMSViewPos; // Direction toward viewpoint

out vec3 vtxNormal;
out vec3 vtxViewDir;

//------------------------------------------------------------------------------
void main ( void )
{
	vtxNormal = normal;
	vtxViewDir = uMSViewPos - vertex;
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;
in vec3 vtxNormal;
in vec3 vtxViewDir;

layout(location = 3) uniform vec3 uAlbedo;

layout(location = 1) uniform vec3 uMSLightDir; // Direction toward light

float ev = 8.0;
vec3 lightColor = 100.0 * vec3(1.0, 1.0, 0.9);
float roughness = 0.9;
float metallic = 0.1;

float PI = 3.14159265359;

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

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

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

//------------------------------------------------------------------------------	
void main (void) {
	vec3 albedo = uAlbedo;
	// Normalize data from vertex
	vec3 msNormal = normalize(vtxNormal);
	vec3 msLightDir = normalize(uMSLightDir);
	vec3 msViewDir = normalize(vtxViewDir);
	
	// Compute illumination intermediate variables
	vec3 msHalfV = normalize(msViewDir+msLightDir);
	float NdL = max(0.0,dot(msLightDir,msNormal));
	float NdV = max(0.0,dot(msViewDir,msNormal));
	float NdH = max(0.0,dot(msHalfV,msNormal));
	
	// Compute actual lighting
	vec3 F0 = vec3(0.04); 
	F0      = mix(F0, albedo, metallic);
	vec3 F  = fresnelSchlick(NdV, F0);
	
	float NDF = GGX(NdH, roughness);
	float G   = GeometrySmith(NdV, NdL, roughness);
	
	vec3 nominator    = NDF * G * F;
	float denominator = 4.0 * NdV * NdL;
	vec3 specular     = nominator / max(denominator, 0.001);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	  
	kD *= 1.0 - metallic;
	
	vec3 ambient = 10.0 * vec3(0.7,0.8,1.0) * albedo;
	vec3 direct = (kD * albedo / PI + specular) * lightColor * NdL;
	outColor = direct + ambient;
	// Tone mapping
	outColor = (outColor / ev);
	outColor = outColor / (vec3(1.0) + outColor);
	//outColor = pow(outColor, vec3(2.2));
	outColor = uAlbedo * vec3(NdL);
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif