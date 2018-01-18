//----------------------------------------------------------------------------------------------------------------------
// Unified PBR shader
//----------------------------------------------------------------------------------------------------------------------
#version 330 core
#define USE_GAMMA_CORRECTION
//#define ALBEDO_MAP
//#define PHYSICS_MAP
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
#define USE_UV0
uniform sampler2D albedoMap;
#else // !ALBEDO_MAP
uniform vec3 albedo;
#endif // !ALBEDO_MAP
#ifdef PHYSICS_MAP
#define USE_UV0
uniform sampler2D uPhysicsMap;
#else // !PHYSICS_MAP
uniform float roughness;
uniform float metalness;
#endif // !PHYSICS_MAP
uniform samplerCube environmentMap;

varying vec2 vUV0;


vec3 lightClr = 4.0*vec3(1,1,0.9);

float PI = 3.14159265359;

float DistributionGGX(float ndh, float a)
{
    float a2     = max(0.01,a*a);
    float ndh2 = ndh*ndh;
	
    float nom    = a2;
    float denom  = (ndh2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

vec2 GeometrySchlickGGX(vec2 ndv, float k)
{
    vec2 nom   = ndv;
    vec2 denom = ndv * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(float ndv, float ndl, float roughness)
{
	float r = roughness+1.0;
	float k = r*r / 8.0;
    vec2 ggx = GeometrySchlickGGX(vec2(ndv,ndl), k);
	
    return ggx.x * ggx.y;
}

vec3 fresnelSchlick(float ndv, vec3 F0)
{
	float oneMinusNdV = 1.0 - ndv;
	float p2 = oneMinusNdV * oneMinusNdV;
	float p3 = oneMinusNdV * p2;
    return F0 + (1.0 - F0) * p3*p2;
}

vec4 fragment_shader(vec3 albedo) {
#ifdef PHYSICS_MAP
	vec2 phy = texture(uPhysicsMap, vUV0).xz;
	float roughness = phy.x;
	float metalness = phy.y;
#endif // PHYSICS_MAP
	// Tint reflections for metals
	vec3 F0 = mix(vec3(0.01), albedo, metalness);
	//// BRDF params
	vec3 viewDir = normalize(vViewDir);
	vec3 normal = normalize(vNormal);
	vec3 halfV = -normalize(uLightDir + viewDir);
	float ndh = max(0.0, dot(halfV, normal));
	float ndl = max(0.0, dot(-uLightDir, normal));
	float ndv = max(0.0, dot(-viewDir, normal));
	// Specular
	vec3 Fs = fresnelSchlick(ndv, F0);
	float NDF = DistributionGGX(ndh, roughness);
	float G = GeometrySmith(ndv, ndl, roughness);
	
	vec3 kS = Fs;
	vec3 kD = vec3(1.0)-kS;
	kD *= 1-metalness;
	
	vec3 nom = NDF * G * Fs;
	float den = max(4.0 * ndv * ndl, 0.001);
	vec3 spec = nom / den;
	vec3 L0 = (kD*albedo / PI + spec) * lightClr *ndl;
	
	vec3 reflDir = reflect(viewDir, normal);
	vec3 env = textureLod(environmentMap, reflDir, 10*roughness).xyz;
	vec3 ambient = textureLod(environmentMap, normal, 6).xyz;
	
	vec3 indirect = ambient*kD + env *kS;
	
	return vec4(L0+indirect, 1.0);
	//return vec4(L0, 1.0);
}

// ----- Vertex shader -----
#ifdef VERTEX_SHADER
// Attributes
attribute vec3 vertex;
attribute vec3 normal;
#ifdef USE_UV0
attribute vec2 uv;
#endif // USE_UV0

void main ( void )
{
	vNormal = normal;
	vViewDir = normalize(vertex - uViewPos);
#ifdef USE_UV0
	vUV0 = uv;
#endif // USE_UV0
	gl_Position = uWorldViewProj * vec4(vertex, 1.0);
}
#endif // VERTEX_SHADER

// ----- Fragment shader -----
#ifdef FRAGMENT_SHADER

void main (void) {
#ifdef ALBEDO_MAP
	vec3 albedo = texture(albedoMap, vUV0).xyz;
#endif // ALBEDO_MAP
	vec4 linearColor = max(fragment_shader(albedo), vec4(0));
	//linearColor = linearColor / (linearColor + vec3(1.0));
#ifdef USE_GAMMA_CORRECTION
	vec3 gammaColor = pow(linearColor.xyz, vec3(0.4545));
	gl_FragColor = vec4(gammaColor, linearColor.a);
#else
	gl_FragColor = linearColor;
#endif
}
#endif // FRAGMENT_SHADER