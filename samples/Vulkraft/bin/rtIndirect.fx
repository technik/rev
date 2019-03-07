#include "rt_common.fx"

layout(location = 3) uniform sampler2D uGBuffer;
layout(location = 4) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

vec3 indirectContrib(in vec3 ro, in vec3 normal, in vec4 noise)
{
	vec3 directLight = vec3(0.0);
	// Scatter reflected light
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 noise = texelFetch(uNoise, pixel_coords%64, 0);

	// Trace ray to the sky to gather light contribution
	// Idea: Can store a spherical harmonic of surrounding illumination and use it to importance sample the environment
	// Maybe that precomputation can improve convergence for indoor scenes where sky is almost always covered

	// Diffuse lighting
	vec3 rd = lambertianDirection(normal, noise.wz);

	float tMax = 100.0;
	vec3 tNormal; // ignored
	float t = hit(ro, rd, tNormal, tMax);
	if(t > 0.0)
	{
		vec3 secondPoint = ro+0.001*tNormal + t*rd;
		directLight = directContrib(secondPoint, tNormal, noise);
	}
	return vec3(0.5);
}

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

	pixel.xyz = vec3(100.5);//indirectContrib(ro, normal);
	pixel.w = 1.0;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}