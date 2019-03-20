#ifdef PXL_SHADER

// Global state
layout(location = 0) uniform vec4 uWindow;
layout(location = 1) uniform sampler2D uRayTracedBuffer;;

//------------------------------------------------------------------------------	
vec3 shade () {
	vec2 uv = gl_FragCoord.xy / uWindow.xy;
	vec4 tBuffer = texture(uRayTracedBuffer, uv);
	vec3 color = 0.5*tBuffer.xyz;

	color = color / (1+color);

	return pow(color, vec3(0.4545));
}

#endif