
#ifdef PXL_SHADER

// Global state
layout(location = 0) uniform mat4 invView;
layout(location = 1) uniform mat4 proj;
layout(location = 2) uniform vec4 Window;
layout(location = 3) uniform float uEV;
layout(location = 7) uniform sampler2D hdrSkyTexture;

//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.159154943, 0.31830988);
vec2 sampleSpherical(vec3 v)
{
  vec2 uv = vec2(atan(-v.x, v.z), asin(-v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//------------------------------------------------------------------------------	
vec3 shade () {
	mat4 invProj = inverse(proj);
	vec2 uv = 2*gl_FragCoord.xy / Window.xy - 1;
	// Direction from the view point to the pixel, in world space
	vec4 vsPixelPos = invProj * vec4(uv, 1.0, 1.0);
	vec3 wsViewDir = (invView * vec4(vsPixelPos.xyz, 0.0)).xyz;
	vec3 color = textureLod(hdrSkyTexture, sampleSpherical(normalize(wsViewDir)), 0.0).xyz;
	
	return pow(pow(2, uEV) * color, vec3(0.4545));
}

#endif