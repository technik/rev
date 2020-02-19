// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Inputs
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform float uStep;
layout(location = 10) uniform sampler2D uInput;
layout(location = 11) uniform sampler2D uGBuffer;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D outBuffer;

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

	vec4 leftNormal = texelFetch(uGBuffer, left_coords, 0);
	if(leftNormal.w > 0)
	{
		float lweight = 0.71*max(0.0, dot(leftNormal.xyz,normal.xyz));
		value += texelFetch(uInput, left_coords, 0) * lweight;
		weight += lweight;
	}

	vec4 rightNormal = texelFetch(uGBuffer, right_coords, 0);
	if(rightNormal.w > 0)
	{
		float rweight = 0.71*max(0.0, dot(rightNormal.xyz,normal.xyz));
		value += texelFetch(uInput, right_coords, 0) * rweight;
		weight += rweight;
	}

	// output to a specific pixel in the image
	imageStore(outBuffer, pixel_coords, value / weight);
}