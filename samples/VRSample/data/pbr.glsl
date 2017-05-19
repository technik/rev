//----------------------------------------------------------------------------------------------------------------------
// Unified PBR shader
//----------------------------------------------------------------------------------------------------------------------
#define USE_GAMMA_CORRECTION
// ----- Common code -----
// Uniforms
uniform	mat4	uWorldViewProj;
uniform	vec3	uViewPos;
uniform vec3 	uLightDir; // In object space
uniform vec3 	uLightClr;

// Varyings
varying vec3 vNormal;
varying vec3 vViewDir;

vec3 lightClr = vec3(1);

vec4 fragment_shader() {
	float reflective = 1.0;
	vec3 albedo = vec3(0.8,0.8,0.8);
	vec3 normal = normalize(vNormal);
	vec3 halfV = -normalize(uLightDir + vViewDir);
	float spec = max(0.0, dot(halfV, normal));
	spec *= spec;
	spec *= spec * reflective;
	float diff = max(0.0, dot(-uLightDir, normal));
	diff *= 1-reflective;
	vec3 color = lightClr * ((albedo*diff)+vec3(spec));
	return vec4(color , 1.0);
}

// ----- Vertex shader -----
#ifdef VERTEX_SHADER
// Attributes
attribute vec3 vertex;
attribute vec3 normal;

void main ( void )
{
	vNormal = normal;
	vViewDir = normalize(vertex - uViewPos);
	gl_Position = uWorldViewProj * vec4(vertex, 1.0);
}
#endif // VERTEX_SHADER

// ----- Fragment shader -----
#ifdef FRAGMENT_SHADER
void main (void) {
#ifdef USE_GAMMA_CORRECTION
	vec4 linearColor = max(fragment_shader(), vec4(0));
	vec3 gammaColor = pow(linearColor.xyz, vec3(0.4545));
	gl_FragColor = vec4(gammaColor, linearColor.a);
#else
	gl_FragColor = fragment_shader();
#endif
}
#endif // FRAGMENT_SHADER