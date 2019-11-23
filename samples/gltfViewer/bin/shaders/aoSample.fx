// AO - Sampling pass
#ifdef PXL_SHADER

// Global state
layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 invView;
layout(location = 2) uniform vec4 Window;

layout(location = 7) uniform sampler2D uGBuffer;
layout(location = 8) uniform sampler2D uDepthMap;

vec4 vsPosFromGBuffer(float depthBufferSample, vec2 uv)
{
	float bufferDepth = depthBufferSample*2-1;
    float B = proj[3][2];
    float zView = -B / (bufferDepth+1);
	vec3 csPos = vec3(uv*2-1, bufferDepth)*-zView;
	return inverse(proj) * vec4(csPos, -zView);
}

vec4 wsPosFromGBuffer(float depthBufferSample, vec2 uv)
{
	return invView * vsPosFromGBuffer(depthBufferSample, uv);
}

vec2 samplePos[4] = 
{
	vec2(-1.0, 0.0),
	vec2(1.0, 0.0),
	vec2(0.0, -1.0),
	vec2(0.0, 1.0)
};

//------------------------------------------------------------------------------	
vec3 shade () {
	//mat4 invViewProj = inverse(proj);
	vec2 uv = gl_FragCoord.xy / Window.xy;
	// Direction from the view point to the pixel, in view space
	vec3 compressedNormal = textureLod(uGBuffer, uv, 0.0).xyz;
	vec3 wsNormal = normalize(compressedNormal*2.0-1.0);

	float sampleDepth = texture(uDepthMap, uv).x;
	vec4 vsPos = vsPosFromGBuffer(sampleDepth, uv);
	vec4 wsPos = invView * vsPos;

	float w = 0.0;
	for(int i = 0; i < 4; ++i)
	{
		vec2 sampleUV = uv + 3.f * samplePos[i] / Window.xy;
		float occluderDepth = texture(uDepthMap, sampleUV).x;
		vec4 occlPos = wsPosFromGBuffer(occluderDepth, sampleUV);
		vec3 sampleFromCenter = (occlPos - wsPos).xyz;
		float d = dot(sampleFromCenter, wsNormal);
		if( d > 0.0 && d/abs(vsPos.z) < 0.004)
		{
			vec3 sampleDir = normalize(sampleFromCenter);
			w += 1-dot(sampleDir, wsNormal);
		}
		else
		{
			w += 1.0;
		}
	}
	w = w/4;

    return vec3(w);
}

#endif