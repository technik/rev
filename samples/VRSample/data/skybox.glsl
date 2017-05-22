//----------------------------------------------------------------------------------------------------------------------
// Skybox shader
//----------------------------------------------------------------------------------------------------------------------
#define USE_GAMMA_CORRECTION
// ----- Common code -----
// Uniforms
uniform	mat4	uWorldViewProj;
uniform	vec3	uViewPos;
uniform vec3 	uLightDir; // In object space
uniform vec3 	uLightClr;

// Varyings
varying vec3 vPos;

vec3 skyColor(vec3 dir) {
	return vec3(-dir.z*10,0.4+dir.z*0.4,dir.z*10);
}

vec4 fragment_shader() {
	vec3 L0 = skyColor(normalize(vPos));
	return vec4(L0, 1.0);
}

// ----- Vertex shader -----
#ifdef VERTEX_SHADER
// Attributes
attribute vec3 vertex;
attribute vec3 normal;

void main ( void )
{
	vPos = vertex;
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