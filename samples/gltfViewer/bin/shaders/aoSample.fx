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
const float maxWsDistance = 10;

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

float sliceGTAO(
    in vec2 ssSampleDir,
    in vec2 uvCenter,
    in vec3 vsCenter,
    in vec3 vsNormal)
{
    // Try to get mostly resolution independent sampling.
    const float appxTanFov = 1.3; // TODO: Pass this as an uniform
	const float maxPixelDistance =
        min(128,-maxWsDistance*Window.y/(vsCenter.z*appxTanFov));
	const float sampleDelta = max(1.4,maxPixelDistance/10);
    // Project sample direction over the plane perpendicular to
    // the normal
    vec3 vsSampleDir = vec3(ssSampleDir, 0.0);
    float nds = dot(vsNormal,vsSampleDir);
    vec3 planarSampleDir = vsSampleDir-vsNormal*nds;
    // Same thing with z dir
    vec2 pixelCenter = uvCenter*Window.xy;
    float ndz = vsNormal.z;
    vec3 planarZDir = vec3(0.0,0.0,1.0)-vsNormal*ndz;
    float sinSign = sign(dot(planarZDir,planarSampleDir));
    float maxCosH1 = 0;
    float maxCosH2 = 0;
    for(float ssDistance = sampleDelta; ssDistance <= maxPixelDistance; ssDistance += sampleDelta)
    {
        vec2 ssSampleDelta = ssDistance*ssSampleDir;
        vec2 uvDelta = ssSampleDelta / Window.xy;
        // Sample +dir
        ivec2 samplePxl1 = ivec2(pixelCenter+ssSampleDelta);
        ivec2 samplePxl2 = ivec2(pixelCenter-ssSampleDelta);
        vec2 sampleUV1 = uvCenter+uvDelta;
        vec2 sampleUV2 = uvCenter-uvDelta;
        float occluderDepth1 =
            texelFetch(uDepthMap, samplePxl1, 0).x;
        float occluderDepth2 =
            texelFetch(uDepthMap, samplePxl2, 0).x;
        float vsOccluderZ1 = vsPosFromGBuffer(occluderDepth1, sampleUV1).z;
        float vsOccluderZ2 = vsPosFromGBuffer(occluderDepth2, sampleUV2).z;
        float dz1 = vsOccluderZ1-vsCenter.z;
        float dz2 = vsOccluderZ2-vsCenter.z;
        float projectedH1 = ndz*dz1+nds*ssDistance;
        float projectedH2 = ndz*dz2+nds*ssDistance;
        float ssDistance2 = ssDistance*ssDistance;
        float d21 = dz1*dz1+ssDistance2;
        float d22 = dz2*dz2+ssDistance2;
        float cosH12 = projectedH1*projectedH1/d21;
        float cosH22 = projectedH2*projectedH2/d22;
        // TODO: This doesn't work well for h < 0
        if(cosH12 > maxCosH1)
        {
            maxCosH1 = cosH12;
        }
        // TODO: This doesn't work well for h < 0
        if(cosH22 > maxCosH2)
        {
            maxCosH2 = cosH22;
        }
    }
    return 0.25*((1-maxCosH1*maxCosH1)+(1-maxCosH2*maxCosH2));
}

vec2 upVec2(float u)
{
    float x = 2*u-1;
    return vec2(x, sqrt(1-x*x));
}

//------------------------------------------------------------------------------	
vec3 shade () {
	ivec2 pixelPos = ivec2(gl_FragCoord.xy);
	vec2 uv = gl_FragCoord.xy / Window.xy;
	// Direction from the view point to the pixel, in view space
	vec3 compressedNormal = texelFetch(uGBuffer, pixelPos, 0).xyz;
	vec3 wsNormal = normalize(compressedNormal*2.0-1.0);
    vec3 vsNormal = (inverse(invView) * vec4(wsNormal,0.0)).xyz;

	float sampleDepth = texelFetch(uDepthMap, pixelPos, 0).x;

	vec4 vsPos = vsPosFromGBuffer(sampleDepth, uv);
	vec4 seeds = texelFetch(uNoise, pixelPos%64, 0);

	float w = 0.0;
	const int nSamples = 4;
	
    for(int i = 0; i < nSamples; ++i)
    {
        vec2 ssSampleDir = upVec2(seeds[i%4]); // Random sample direction
        w += sliceGTAO(ssSampleDir, uv, vsPos.xyz, vsNormal);
    }

    return vec3(w/nSamples);
}

#endif