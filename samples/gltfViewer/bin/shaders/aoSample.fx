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
	vec4 wsPos = invView * vsPos;

	vec4 seeds = texelFetch(uNoise, pixelPos%64, 0);

	float w = 0.0;
	float aoMaxDistance = 20.f;
	for(int i = 0; i < 4; ++i)
	{
		float u = seeds[i]*2-1;
		float v = sqrt(1-u*u);
		vec2 samplePos = vec2(u,v);
		vec2 sampleUV = uv + aoMaxDistance * samplePos / Window.xy;
		float occluderDepth = textureLod(uDepthMap, sampleUV, 0).x;
		vec4 occlPos = wsPosFromGBuffer(occluderDepth, sampleUV);
		vec3 sampleFromCenter = (occlPos - wsPos).xyz;
		float d = dot(sampleFromCenter, wsNormal);
		if( d > 0.0 && d/abs(vsPos.z) < 0.02)
		{
			vec3 sampleDir = normalize(sampleFromCenter);
			w += 1-dot(sampleDir, wsNormal);
		}
		else
		{
			w += 1.0;
		}

		sampleUV = uv - aoMaxDistance * samplePos / Window.xy;
		occluderDepth = textureLod(uDepthMap, sampleUV, 0).x;
		occlPos = wsPosFromGBuffer(occluderDepth, sampleUV);
		sampleFromCenter = (occlPos - wsPos).xyz;
		d = dot(sampleFromCenter, wsNormal);
		if( d > 0.0 && d/abs(vsPos.z) < 0.02)
		{
			vec3 sampleDir = normalize(sampleFromCenter);
			w += 1-dot(sampleDir, wsNormal);
		}
		else
		{
			w += 1.0;
		}
	}
	w = w/8;

    return vec3(w);
}

#endif