
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
#define OVERRIDE_LGS

#include "rt_common.fx"

// Raytrace a g-buffer with surface normal and distance from the camera
// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D gBufferOut;

void main() {
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	if(gl_GlobalInvocationID.x >= uWindow.x || gl_GlobalInvocationID.y >= uWindow.y)
		return;
	//
	// Compute uvs
	vec3 ro, rd;
	worldRayFromPixel(uCamWorld, pixel_coords, ro, rd);

	float tMax = 1000.0;
	vec3 normal = vec3(0.0, 0.0, -1.0);
	float t = hit(ro, rd, normal, tMax);
	//vec4 gBufferPixel = vec4(vec3(rd.y), 1.0);
	vec4 gBufferPixel = vec4(normal,t);

	// output to a specific pixel in the image
	imageStore(gBufferOut, pixel_coords, gBufferPixel);
}