// Local work group
layout(local_size_x = 32, local_size_y = 1) in;

// Inputs
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform vec4 uStep;
layout(location = 10) uniform sampler2D uInput;
layout(location = 11) uniform sampler2D uGBuffer;

#include "math.fx"

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D outBuffer;

float depthWeight(float depth0, float depth1)
{
	float z0 = zFromDepthBuffer(uProj, depth0);
	float z1 = zFromDepthBuffer(uProj, depth1);
	float depthError = abs(z1-z0);
	const float maxRelativeError = 1;
	return max(0.0, 1-depthError/maxRelativeError);
}

void boxAccum(
	in ivec2 centerCoords,
	in ivec2 sampleCoords,
	in vec4 gBufferCenter,
	inout float weight,
	inout vec4 value)
{
	if(centerCoords == sampleCoords)
		return;

	vec4 sampleGBuffer = texelFetch(uGBuffer, sampleCoords, 0);
	float sampleDepth = sampleGBuffer.w;
	if(sampleDepth < 0)
		return;

	float centerDepth = gBufferCenter.w;
	const float minNormalDot = 0.5;
	float normalWeight = max(0.0, dot(sampleGBuffer.xyz,gBufferCenter.xyz)-minNormalDot);
	float distanceWeight = depthWeight(centerDepth, sampleDepth);

	float sampleWeight = 0.7 * normalWeight *distanceWeight;
	if(sampleWeight > 0)
	{
		weight += sampleWeight;
		value += sampleWeight * texelFetch(uInput, sampleCoords, 0);
	}
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	if(pixel_coords.x >= uWindow.x || pixel_coords.y >= uWindow.y)
		return;

	vec4 normal = texelFetch(uGBuffer, pixel_coords, 0);
	if(normal.w < 0)
	{
		imageStore(outBuffer, pixel_coords, vec4(0));
		return;
	}

	vec4 value = texelFetch(uInput, pixel_coords, 0);
	float weight = 1.0;

	ivec2 neg_coords = ivec2(max(vec2(0),pixel_coords-uStep.xy));
	boxAccum(pixel_coords, neg_coords, normal, weight, value);

	ivec2 pos_coords = ivec2(min(uWindow.xy-1,pixel_coords+uStep.xy));
	boxAccum(pixel_coords, pos_coords, normal, weight, value);

	// output to a specific pixel in the image
	imageStore(outBuffer, pixel_coords, value / weight);
}