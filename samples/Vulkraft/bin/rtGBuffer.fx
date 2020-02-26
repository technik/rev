#include "rt_common.fx"

// Raytrace a g-buffer with surface normal and distance from the camera
// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D gBufferOut;

void main() {
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//
	// Compute uvs
	vec2 uvs = 2*vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y) / uWindow.xy-1;
	vec3 ro = -(transpose(uViewMtx) * uViewMtx[3]).xyz;
	vec4 vsRd = viewSpaceRay(uvs);
	vec3 rd = (uCamWorld * vsRd).xyz;

	float tMax = 1000.0;
	vec3 normal = vec3(0.0, 0.0, -1.0);
	vec3 hitPoint;
	float t = hit(ro, rd, normal, hitPoint, tMax);
	//vec4 gBufferPixel = vec4(vec3(rd.y), 1.0);
	float d = -t*vsRd.z;
	vec4 gBufferPixel = vec4(normal,d);

	// output to a specific pixel in the image
	imageStore(gBufferOut, pixel_coords, gBufferPixel);
}