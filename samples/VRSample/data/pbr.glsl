//----------------------------------------------------------------------------------------------------------------------
// Unified PBR shader
//----------------------------------------------------------------------------------------------------------------------
#define USE_GAMMA_CORRECTION
#define ALBEDO_MAP
// ----- Common code -----
// Uniforms
uniform	mat4	uWorldViewProj;
uniform	vec3	uViewPos;
uniform vec3 	uLightDir; // In object space
uniform vec3 	uLightClr;

// Varyings
varying vec3 vNormal;
varying vec3 vViewDir;

#ifdef ALBEDO_MAP
uniform sampler2D uAlbedoMap;
varying vec2 vUV0;
#else // !ALBEDO_MAP
uniform vec3 uAlbedo;
#endif // !ALBEDO_MAP

vec3 lightClr = vec3(1,1,0.9);

float PI = 3.14159;

float DistributionGGX(float ndh, float a)
{
    float a2     = a*a;
    float ndh2 = ndh*ndh;
	
    float nom    = a2;
    float denom  = (ndh2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float ndv, float k)
{
    float nom   = ndv;
    float denom = ndv * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(float ndv, float ndl, float k)
{
    float ggx1 = GeometrySchlickGGX(ndv, k);
    float ggx2 = GeometrySchlickGGX(ndl, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float ndv, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - ndv, 5.0);
}

vec4 fragment_shader(vec3 albedo) {
	float reflectivity = 0.9;
	float metalness = 0.01;
	float roughness = 0.1;
	// Tint reflections for metals
	vec3 F0 = mix(vec3(reflectivity), albedo, metalness);
	//// BRDF params
	vec3 normal = normalize(vNormal);
	vec3 halfV = -normalize(uLightDir + vViewDir);
	float ndh = max(0.0, dot(halfV, normal));
	float ndl = max(0.0, dot(-uLightDir, normal));
	float ndv = max(0.0, dot(-vViewDir, normal));
	// Specular
	vec3 Fs = fresnelSchlick(ndv, F0);
	float NDF = DistributionGGX(ndh, roughness);
	float G = GeometrySmith(ndv, ndl, roughness);
	
	vec3 Ks = Fs;
	vec3 Kd = vec3(1)-Ks;
	Kd *= 1-metalness;
	
	vec3 nom = NDF * G * Fs;
	float den = 1.0/(4*ndv*ndl*1.0);
	vec3 spec = nom * den;
	vec3 L0 = (Kd*albedo / PI + spec) * lightClr * ndl;
	return vec4(L0, 1.0);
}

// ----- Vertex shader -----
#ifdef VERTEX_SHADER
// Attributes
attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 uv;

void main ( void )
{
	vNormal = normal;
	vViewDir = normalize(vertex - uViewPos);
	vUV0 = uv;
	gl_Position = uWorldViewProj * vec4(vertex, 1.0);
}
#endif // VERTEX_SHADER

// ----- Fragment shader -----
#ifdef FRAGMENT_SHADER

void main (void) {
#ifdef ALBEDO_MAP
	vec3 albedo = texture(uAlbedoMap, vUV0).xyz;
#else //!ALBEDO_MAP
	vec3 albedo = uAlbedo;
#endif // !ALBEDO_MAP
	
#ifdef USE_GAMMA_CORRECTION
	vec4 linearColor = max(fragment_shader(albedo), vec4(0));
	vec3 gammaColor = pow(linearColor.xyz, vec3(0.4545));
	gl_FragColor = vec4(gammaColor, linearColor.a);
#else
	gl_FragColor = fragment_shader();
#endif
}
#endif // FRAGMENT_SHADER