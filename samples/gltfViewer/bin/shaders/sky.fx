
#ifdef PXL_SHADER

// Global state
layout(location = 0) uniform mat4 viewProj;
layout(location = 1) uniform vec4 Window;
layout(location = 3) uniform float uEV;
layout(location = 7) uniform sampler2D hdrSkyTexture;

//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSpherical(vec3 v)
{
  vec2 uv = vec2(atan(-v.x, v.z), asin(-v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//------------------------------------------------------------------------------	
vec3 shade () {
	mat4 invViewProj = inverse(viewProj);
	vec2 uv = 2*gl_FragCoord.xy / Window.xy - 1;
	// Direction from the view point to the pixel, in world space
	vec3 vtxViewDir = (invViewProj * vec4(uv, 1.0, 1.0)).xyz;
	vec3 color = textureLod(hdrSkyTexture, sampleSpherical(normalize(vtxViewDir)), 0.0).xyz;
	
	color.xyz = color.xyz / (0.4+color.xyz);
	return pow(pow(2, uEV) * color, vec3(0.4545));
	//return color.r > 1.0 ? vec3(1.0,0.0,0.0) : color;//pow(color, vec3(1.0004545));
}

#endif