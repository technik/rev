
#ifdef PXL_SHADER

// Global state
layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 invView;
layout(location = 2) uniform vec4 Window;

layout(location = 4) uniform sampler2D uEnvironment;
layout(location = 5) uniform sampler2D uEnvBRDF;
layout(location = 6) uniform float numEnvLevels;

layout(location = 7) uniform sampler2D uGBuffer;
layout(location = 8) uniform sampler2D uDepthMap;
layout(location = 9) uniform sampler2D uSpecularMap;
layout(location = 10) uniform sampler2D uAlbedoMap;

#define sampler2D_uShadowMap
layout(location = 11) uniform sampler2D uShadowMap;
layout(location = 12) uniform mat4 uShadowProj;

#define sampler2D_uEnvironment

#include "ibl.fx"

//------------------------------------------------------------------------------	
vec3 shade () {
	//mat4 invViewProj = inverse(proj);
	vec2 uv = gl_FragCoord.xy / Window.xy;
	// Direction from the view point to the pixel, in view space
	vec3 compressedNormal = textureLod(uGBuffer, uv, 0.0).xyz;
	vec3 wsNormal = normalize(compressedNormal*2.0-1.0);

	float fragDepth = texture(uDepthMap, uv).x;
    float bufferDepth = fragDepth*2-1;
    float B = proj[3][2];
    float zView = -B / (bufferDepth+1);
	vec3 csPos = vec3(uv*2-1, bufferDepth)*-zView;
	vec4 vsPos = inverse(proj) * vec4(csPos, -zView);
	vec4 wsPos = invView * vsPos;
	vec4 wsEyePos = invView * vec4(vec3(0.0), 1.0);

	vec3 wsEyeDir = normalize(wsEyePos.xyz-wsPos.xyz);

	vec4 f0_roughness = texture(uSpecularMap, uv);
	vec4 albedo_ao = texture(uAlbedoMap, uv);
	vec3 albedo = albedo_ao.xyz;
	vec3 F0 = f0_roughness.xyz;
	float r = f0_roughness.a;
	float occlusion = albedo_ao.w;
#ifdef sampler2D_uShadowMap
	vec4 shadowPos = uShadowProj * wsPos;
	float shadowDepth = textureLod(uShadowMap, shadowPos.xy*0.5+0.5, 0.0).x;
    float surfaceDepth = shadowPos.z*0.5+0.5;
    float shadow = (shadowDepth > surfaceDepth) ? 0.0 : 1.0;
    vec3 lightDir = (inverse(uShadowProj)*vec4(0,0,-1,0)).xyz;
    lightDir = normalize(lightDir);
#else
	float shadow = 1.0;
#endif
	float ndv = max(0.0, dot(wsEyeDir, wsNormal));
	shadow = 0.2+0.8*shadow;
	return ibl(F0, wsNormal, wsEyeDir, albedo, lightDir, r, occlusion, shadow, ndv);
}

#endif