layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform float uStep;
layout(location = 10) uniform sampler2D uInput;
layout(location = 11) uniform sampler2D uGBuffer;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D outBuffer;

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 up_coords = ivec2(pixel_coords.x, max(0,pixel_coords.y-uStep));
	ivec2 down_coords = ivec2(pixel_coords.x, min(uWindow.y-1,pixel_coords.y+uStep));

	vec4 normal = texelFetch(uGBuffer, pixel_coords, 0);
	if(normal.w < 0)
	{
		imageStore(outBuffer, pixel_coords, vec4(0));
		return;
	}

	vec4 value = texelFetch(uInput, pixel_coords, 0);
	float weight = 1.0;

	vec4 upNormal = texelFetch(uGBuffer, up_coords, 0);
	if(upNormal.w > 0)
	{
		float uweight = max(0.0, dot(upNormal.xyz,normal.xyz));
		value += texelFetch(uInput, up_coords, 0) * uweight;
		weight += uweight;
	}

	vec4 downNormal = texelFetch(uGBuffer, down_coords, 0);
	if(downNormal.w > 0)
	{
		float dweight = max(0.0, dot(downNormal.xyz,normal.xyz));
		value += texelFetch(uInput, down_coords, 0) * dweight;
		weight += dweight;
	}

	// output to a specific pixel in the image
	imageStore(outBuffer, pixel_coords, value / weight);
}