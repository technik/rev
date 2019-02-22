#include "rt_common.fx"

layout(location = 3) uniform sampler2D uGBuffer;
layout(location = 4) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//
	// Compute uvs
	vec2 uvs = vec2(pixel_coords.x, pixel_coords.y) * (1/uWindow.xy);
	vec2 xy = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 ro = (uView * vec4(0,0,0,1.0)).xyz;
	vec3 rd = (uView * vec4(normalize(vec3(xy.x, xy.y, -2.0)), 0.0)).xyz;
	vec4 gBufferData = textureLod(uGBuffer, uvs, 0);
	if(gBufferData.w < 0.0)
		return;
	vec3 normal = gBufferData.xyz;
	ro = ro + gBufferData.w * rd + 1e-5 * normal;

	// Scatter reflected light
	float noiseX = float(gl_GlobalInvocationID.x) / 64;
	float noiseY = float(gl_GlobalInvocationID.y) / 64;
	vec4 noise = textureLod(uNoise, vec2(noiseX, noiseY), 0);
	pixel.xyz = directContrib(ro, normal, noise);
	pixel.w = 1.0;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}