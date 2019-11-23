// AO - Sampling pass
#ifdef PXL_SHADER

// Global state
layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 invView;
layout(location = 2) uniform vec4 Window;

layout(location = 7) uniform sampler2D uGBuffer;
layout(location = 8) uniform sampler2D uDepthMap;
layout(location = 9) uniform sampler2D uNoise;

const float TwoPi = 6.2831852436065673828125f;

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

// Pixar's method for orthonormal basis generation
void branchlessONB(in vec3 n, out vec3 b1, out vec3 b2)
{
	float sign = n.z > 0.0 ? 1.0 : -1.0;
	const float a = -1.0f / (sign + n.z);
	const float b = n.x * n.y * a;
	b1 = vec3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
	b2 = vec3(b, sign + n.y * n.y * a, -n.y);
}

vec3 randomUnitVector(in vec2 seed)
{
	float theta = TwoPi*seed.x;
	float z = 2*seed.y-1;
	float horRad = sqrt(1-z*z);
	return vec3(
		cos(theta)*horRad,
		z,
		sin(theta)*horRad
		);
}

vec3 lambertianDirection(in vec3 normal, in vec2 seed)
{
	return normal + 0.985 * randomUnitVector(seed);
}

vec3 wsMaxHorizon(vec2 uv, in vec3 wsCenter, in vec2 ssDir, in vec3 wsNormal, float depth)
{
	const float maxPixelDistance = 20.0;
	const float sampleDelta = 1.5;

	float bestH = -1.0;
	vec3 bestSamplePos;

	for(float i = 1; i <= maxPixelDistance; ++i)
	{
		float sampleDistance = i * sampleDelta;
		vec2 sampleUV = uv + sampleDistance * ssDir / Window.xy;
		float occluderDepth = textureLod(uDepthMap, sampleUV, 0).x;
		vec3 wsSamplePos = wsPosFromGBuffer(occluderDepth, sampleUV).xyz;
		float h = dot(wsSamplePos-wsCenter, wsNormal) / sampleDistance;
		if(h/depth < 0.001 && h > bestH)
		{
			bestSamplePos = wsSamplePos;
			bestH = h;
		}
	}
	return bestSamplePos;
}

//------------------------------------------------------------------------------	
vec3 shade () {
	//mat4 invViewProj = inverse(proj);
	ivec2 pixelPos = ivec2(gl_FragCoord.xy);
	vec2 uv = gl_FragCoord.xy / Window.xy;
	// Direction from the view point to the pixel, in view space
	vec3 compressedNormal = textureLod(uGBuffer, uv, 0.0).xyz;
	vec3 wsNormal = normalize(compressedNormal*2.0-1.0);

	float sampleDepth = texelFetch(uDepthMap, pixelPos, 0).x;
	vec4 vsPos = vsPosFromGBuffer(sampleDepth, uv);
	vec3 wsPos = (invView * vsPos).xyz;
	vec3 wsEyePos = (invView * vec4(vec3(0.0), 1.0)).xyz;
	vec3 wsEyeDir = normalize(wsEyePos-wsPos);

	vec4 seeds = texelFetch(uNoise, pixelPos%64, 0);

	float w = 0.0;
	int nSamples = 1;

	float ndv = max(0.0, dot(wsEyeDir, wsNormal));
	float sinN = sqrt(1-ndv*ndv);
	for(int i = 0; i < nSamples; ++i)
	{
		float u = seeds[i]*2-1;
		float v = sqrt(1.0-u*u);
		vec2 ssSampleDir = vec2(u,v);
		vec3 wsH1Pos = wsMaxHorizon(uv, wsPos, ssSampleDir, wsNormal, -vsPos.z);
		vec3 wsH1Dir = wsH1Pos-wsPos;
		float cosH1 = max(0.0, dot(normalize(wsH1Dir), wsEyeDir));
		float h1 = acos(cosH1);

		vec3 wsH2Pos = wsMaxHorizon(uv, wsPos, -ssSampleDir, wsNormal, -vsPos.z);
		vec3 wsH2Dir = wsH2Pos-wsPos;
		float cosH2 = max(0.0, dot(normalize(wsH2Dir), wsEyeDir));
		float h2 = acos(cosH2);

		w += 0.25*(2*ndv+2*sinN*(h1+h2)-cos(2*h1-acos(ndv))-cos(2*h2+acos(ndv)));
	}
	w = 0.5*w/nSamples; // 4 Directions, 8 samples

    return vec3(w);
}

#endif