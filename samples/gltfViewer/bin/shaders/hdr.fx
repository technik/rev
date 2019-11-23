// Tone mapping pass
#ifdef PXL_SHADER

// Global state
layout(location = 2) uniform vec4 Window;
layout(location = 3) uniform float uEV;
layout(location = 4) uniform sampler2D uHDR;

//------------------------------------------------------------------------------	
vec3 shade () {
	//mat4 invViewProj = inverse(proj);
	vec2 uv = gl_FragCoord.xy / Window.xy;
	vec3 hdrColor = texture(uHDR, uv).xyz;
	vec3 toneMapped = hdrColor*uEV / (1+uEV*hdrColor);
	//return hdrColor;
    return toneMapped;
}

#endif