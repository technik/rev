// Local work group
layout(local_size_x = 4, local_size_y = 4) in;

#include "math.fx"
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

vec4 viewSpaceRay(mat4 proj, vec2 clipSpacePos)
{
	float zClipDepth = 0.0;
	float wClip = proj[3][2] / (zClipDepth - proj[2][2]/proj[2][3]);
	vec3 csPos = vec3(clipSpacePos.x, clipSpacePos.y, zClipDepth)*wClip;
	vec4 vsPos = inverse(proj) * (vec4(csPos, wClip));
	return vec4(normalize(vsPos.xyz), .0);
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	// Fetch pixel.
	vec4 value = texelFetch(uThisFrame, pixel_coords, 0);
	// No reusable data?
	if(uTaaConfidence > 0) // Reusable data available
	{
		// Fetch G-Buffer. Early out if neg depth
		vec4 curGBuffer = texelFetch(uGBuffer, pixel_coords, 0);
		if(curGBuffer.w > 0)
		{
			vec3 normal = curGBuffer.xyz;
			// Reproject coords
			vec2 uvs = vec2(pixel_coords.x+0.5, pixel_coords.y+0.5) * (1/uWindow.xy);
			vec4 wsRd = viewSpaceRay(uThisViewProj, 2*uvs-1);
			vec4 csOldRd = uPastViewProj * wsRd;
			vec2 pastUVs = csOldRd.xy/csOldRd.w * 0.5 + 0.5;
			// Fetch old GBuffer
			// if valid
			if(uPastViewProj == uThisViewProj)
			{
				vec4 history = texelFetch(uPastFrame, pixel_coords, 0);
				// Accumulate, store and out
				vec4 smoothVal = mix(history, value, 0.0625);
				imageStore(outBuffer, pixel_coords, smoothVal);
				//imageStore(outBuffer, pixel_coords, value);
				return;
			}
		}
	}
	// If we make it here, we couldn't accumulate
	// So just copy this frame's value
	imageStore(outBuffer, pixel_coords, value);
}