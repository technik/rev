// Local work group
layout(local_size_x = 1, local_size_y = 32) in;

// Inputs
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform float uStep;
layout(location = 10) uniform sampler2D uInput;
layout(location = 11) uniform sampler2D uGBuffer;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D outBuffer;

#include "math.fx"

float depthWeight(float depth0, float depth1)
{
	float z0 = zFromDepthBuffer(uProj, depth0);
	float z1 = zFromDepthBuffer(uProj, depth1);
	float depthError = abs(z1-z0);
	const float maxRelativeError = 1;
	return max(0.0, 1-depthError/maxRelativeError);
}

void boxAccum(in ivec2 sampleCoords, in vec4 gBufferCenter, inout float weight, inout vec4 value)
{
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
	if(pixel_coords.y >= uWindow.y)
		return;

	ivec2 up_coords = ivec2(pixel_coords.x, max(0,pixel_coords.y-uStep));
	ivec2 down_coords = ivec2(pixel_coords.x, min(uWindow.y-1,pixel_coords.y+uStep));

	vec4 normal = texelFetch(uGBuffer, pixel_coords, 0);
	float depth = normal.w;
	if(depth < 0)
	{
		imageStore(outBuffer, pixel_coords, vec4(0));
		return;
	}

	vec4 value = texelFetch(uInput, pixel_coords, 0);
	float weight = 1.0;

	boxAccum(up_coords, normal, weight, value);
	boxAccum(down_coords, normal, weight, value);

	// output to a specific pixel in the image
	imageStore(outBuffer, pixel_coords, value / weight);
}