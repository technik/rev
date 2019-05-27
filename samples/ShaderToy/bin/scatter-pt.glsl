#ifdef PXL_SHADER

layout(location = 0) uniform vec4 t;
layout(location = 1) uniform vec4 Window;

vec3 shade () {	
	vec2 uv = gl_FragCoord.xy / Window.xy;

	// TODO: Pathtrace the scene

	// TODO: Tonemapping
	// TODO: Gamma correction
	return vec3(uv.x,uv.y,1.0*sin(t.y));
}

#endif