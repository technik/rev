// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Inputs
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform float uStep;
layout(location = 10) uniform sampler2D uInput;
layout(location = 11) uniform sampler2D uGBuffer;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D outBuffer;

float depthWeight(float depth0, float depth1)
{
	float depthError = abs(depth1-depth0);
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

	float sampleWeight = 0.57 * normalWeight *distanceWeight;
	if(sampleWeight > 0)
	{
		weight += sampleWeight;
		value += sampleWeight * texelFetch(uInput, sampleCoords, 0);
	}
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 left_coords = ivec2(max(0,pixel_coords.x-uStep), pixel_coords.y);
	ivec2 right_coords = ivec2(min(uWindow.x-1,pixel_coords.x+uStep), pixel_coords.y);

	vec4 normal = texelFetch(uGBuffer, pixel_coords, 0);
	if(normal.w < 0)
	{
		imageStore(outBuffer, pixel_coords, vec4(0));
		return;
	}

	vec4 value = texelFetch(uInput, pixel_coords, 0);
	float weight = 1.0;

	boxAccum(left_coords, normal, weight, value);
	boxAccum(right_coords, normal, weight, value);

	// output to a specific pixel in the image
	imageStore(outBuffer, pixel_coords, value / weight);
}