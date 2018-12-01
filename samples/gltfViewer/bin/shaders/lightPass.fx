
#ifdef PXL_SHADER

// Global state
layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 invView;
layout(location = 2) uniform vec4 Window;
layout(location = 3) uniform float uEV;

layout(location = 4) uniform sampler2D uEnvironment;
layout(location = 5) uniform sampler2D uEnvBRDF;
layout(location = 6) uniform float numEnvLevels;

layout(location = 7) uniform sampler2D uGBuffer;
layout(location = 8) uniform sampler2D uDepthMap;
layout(location = 9) uniform sampler2D uSpecularMap;
layout(location = 10) uniform sampler2D uAlbedoMap;

#define sampler2D_uEnvironment

#include "ibl.fx"

out float gl_FragDepth;

//------------------------------------------------------------------------------	
vec3 shade () {
	//mat4 invViewProj = inverse(proj);
	vec2 uv = gl_FragCoord.xy / Window.xy;
	// Direction from the view point to the pixel, in view space
	vec3 compressedNormal = textureLod(uGBuffer, uv, 0.0).xyz;
	vec3 wsNormal = normalize(compressedNormal * 2 - 1);

	gl_FragDepth = texture(uDepthMap, uv).x;
	float zClipDepth = gl_FragDepth*2-1;
	float wClip = proj[3][2] / (zClipDepth - proj[2][2]/proj[2][3]);
	vec2 xyClip = uv*2-1;
	vec3 csPos = vec3(xyClip.x, xyClip.y, zClipDepth);
	vec4 vsPos = (inverse(proj) * vec4(csPos, 1))*wClip;
	vec4 wsPos = invView * vsPos;
	vec4 wsEyePos = invView * vec4(vec3(0.0), 1.0);

	vec3 wsEyeDir = normalize(wsEyePos.xyz-wsPos.xyz);

	vec4 f0_roughness = texture(uSpecularMap, uv);
	vec3 albedo = texture(uAlbedoMap, uv).xyz;
	vec3 F0 = f0_roughness.xyz;
	float r = f0_roughness.a;
	float occlusion = 1.0;
	float shadow = 1.0;
	float ndv = max(0.0, dot(wsEyeDir, wsNormal));
	
	vec3 color = ibl(F0, wsNormal, wsEyeDir, albedo, r, occlusion, shadow, ndv);
	vec3 toneMapped = color*uEV / (1+uEV*color);
	return pow(toneMapped, vec3(0.4545));
}

#endif