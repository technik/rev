#ifdef PXL_SHADER

layout(location = 0) uniform vec4 t;
layout(location = 1) uniform vec4 Window;

vec3 shade () {	
	vec2 uv = (2.0*gl_FragCoord.xy / Window.xy)-1.0;
	return pow(vec3(uv.x,uv.y,t.w), vec3(0.4545));
}

#endif