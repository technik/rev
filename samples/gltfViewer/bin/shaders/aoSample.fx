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

float depthFromPixel(vec2 pixelPos)
{
    return texelFetch(uDepthMap, ivec2(pixelPos), 0).x;
}

vec4 vsPosFromPixel(vec2 pixelPos)
{
    float d = depthFromPixel(pixelPos);
    return vsPosFromGBuffer(d, pixelPos/Window.xy);
}

float horizon(vec3 origin, vec2 ds, vec3 vsNormal)
{
    vec3 occl = vsPosFromPixel(gl_FragCoord.xy+ds.xy).xyz;
    vec3 dx = normalize(occl-origin);
    float ndh = dot(dx,vsNormal);
    if(ndh <= 0) // Not an obstacle
        return 1.0;
    float s2 = dot(ds,ds);
    float s = sqrt(s2);
    float nds = dot(vsNormal.xy, ds)/s;
    if(nds <= 0.0)
    {
        return 1-ndh*ndh;
    }
    else
    {
        float ndz = vsNormal.z;
        float tds = sqrt(1-nds*nds);
        float tdz = sqrt(1-ndz*ndz);
        if(nds>0)
            tdz*=-1;
        float sinH = (dx.z*tdz+s*tds)/sqrt(s2+dx.z*dx.z);
        return (sinH<0?-1.0:1.0)*(1-ndh*ndh);
    }
}

float minHorizonVis(vec3 vsCenter, vec2 dx, vec3 vsNormal)
{
    float minVis = 1.0;
    const int nSamples = 250;
    for(int i = 0; i < nSamples; ++i)
    {
        vec2 x1 = dx*(i+1);
        float h = horizon(vsCenter,x1,vsNormal);
        minVis = min(minVis, h);
    }
    return minVis;
}

float sliceGTAO(
    in vec2 ssSampleDir,
    in vec2 uvCenter,
    in vec3 vsCenter,
    in vec3 vsNormal)
{
    vec3 origin = vsPosFromPixel(gl_FragCoord.xy).xyz;
    // Same thing with z dir
    vec2 ds = 1.414*ssSampleDir;

    float sin2H1 = minHorizonVis(origin, ds, vsNormal);
    float sin2H2 = minHorizonVis(origin,-ds, vsNormal);

    return 0.5*(sin2H1+sin2H2); // Already normalized
}

vec2 upVec2(float u)
{
	float cos2 = abs(2*u-1);
    float x = sqrt(cos2);
    x = u<0.5?-x:x;
    return vec2(x, sqrt(1-cos2));
}

//------------------------------------------------------------------------------	
vec3 shade ()
{
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
	const int nSamples = 1;
	
    for(int i = 0; i < nSamples; ++i)
    {
        vec2 ssSampleDir = upVec2(seeds[i%4]); // Random sample direction
        w += sliceGTAO(ssSampleDir, uv, vsPos.xyz, vsNormal);
    }

    return vec3(w/nSamples);
}

#endif