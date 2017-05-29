//----------------------------------------------------------------------------------------------------------------------
// Skybox shader
//----------------------------------------------------------------------------------------------------------------------
#define USE_GAMMA_CORRECTION
// ----- Common code -----
// Uniforms
uniform	mat4		uWorldViewProj;
uniform samplerCube albedo;

// Varyings
varying vec3 vPos;

vec3 skyColor(vec3 dir) {
	return vec3(-dir.z*10,0.4+dir.z*0.4,dir.z*10);
}

vec4 fragment_shader() {
	return texture(albedo, vPos);
}

// ----- Vertex shader -----
#ifdef VERTEX_SHADER
// Attributes
attribute vec3 vertex;

void main ( void )
{
	vec4 pos = uWorldViewProj * vec4(vertex, 1.0);
	vPos = vertex;
	gl_Position = pos.xyww;
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