// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Inputs
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform mat4 uPastViewProj;
layout(location = 3) uniform mat4 uThisViewProj;
layout(location = 4) uniform float uTaaConfidence;
layout(location = 10) uniform sampler2D uGBuffer;
layout(location = 11) uniform sampler2D uPastFrame;
layout(location = 12) uniform sampler2D uThisFrame;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D outBuffer;

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	// Fetch pixel.
	vec4 value = texelFetch(uInput, pixel_coords, 0);
	// No reusable data?
	if(uTaaConfidence > 0) // Reusable data available
	{
		// Fetch G-Buffer. Early out if neg depth
		vec4 normal = texelFetch(uGBuffer, pixel_coords, 0);
		if(normal.w > 0)
		{
			// Reproject coords
			// Fetch old GBuffer
			// if valid
			{
				// Accumulate, store and out
				float smoothVal = mix(history, value, 0.5);
				imageStore(outBuffer, pixel_coords, value);
			}
		}
	}
	// If we make it here, we couldn't accumulate
	// So just copy this frame's value
	imageStore(outBuffer, pixel_coords, value);
}