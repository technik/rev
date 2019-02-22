#include "rt_common.fx"
// Raytrace a g-buffer with surface normal and distance from the camera
// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D gBufferOut;

void main() {
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//
	// Compute uvs
	vec2 uvs = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 ro = (uView * vec4(0,0,0,1.0)).xyz;
	vec3 rd = (uView * vec4(normalize(vec3(uvs.x, uvs.y, -2.0)), 0.0)).xyz;

	float tMax = 1000.0;
	vec3 albedo; // Ignored
	vec3 normal = vec3(0.0, 0.0, -1.0);
	float t = hit(ro, rd, normal, albedo, tMax);
	vec4 gBufferPixel = vec4(normal,t);

	// output to a specific pixel in the image
	imageStore(gBufferOut, pixel_coords, gBufferPixel);
}