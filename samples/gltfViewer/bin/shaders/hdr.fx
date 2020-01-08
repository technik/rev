// Tone mapping pass
#ifdef PXL_SHADER

// Global state
layout(location = 2) uniform vec4 Window;
layout(location = 3) uniform float uEV;
layout(location = 4) uniform sampler2D uHDR;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

vec3 Reinhard(vec3 x)
{
	return x / (1+x);
}

//------------------------------------------------------------------------------	
vec3 shade () {
	//mat4 invViewProj = inverse(proj);
	vec2 uv = gl_FragCoord.xy / Window.xy;
	vec3 hdrColor = texture(uHDR, uv).xyz;
	vec3 exposedColor = hdrColor*uEV;
	//return hdrColor;
    return ACESFilm(exposedColor);
    //return Reinhard(exposedColor);
}

#endif