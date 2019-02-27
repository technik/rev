#include "rt_common.fx"

layout(location = 3) uniform sampler2D uGBuffer;
layout(location = 4) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

vec3 indirectContrib(in vec3 ro, in vec3 normal)
{
	vec3 directLight = vec3(0.0);
	// Scatter reflected light
	float noiseX = float(gl_GlobalInvocationID.x) / 64;
	float noiseY = float(gl_GlobalInvocationID.y) / 64;
	vec4 noise = textureLod(uNoise, vec2(noiseX, noiseY), 0);

	// Sample a random location in the unit hemisphere
	/*rd = randomUnitVector(noise.xy); // Specular lighting
	if(dot(rd,normal) < 0)
		rd = -rd;*/

	// Trace ray to the sky to gather light contribution
	// Idea: Can store a spherical harmonic of surrounding illumination and use it to importance sample the environment
	// Maybe that precomputation can improve convergence for indoor scenes where sky is almost always covered

	// Diffuse lighting
	vec3 rd = randomUnitVector(noise.xy);
	if(dot(rd,normal) < 0)
		rd = -rd;
	float tMax = 1000.0;
	vec3 albedo, tNormal; // ignored
	float t = hit(ro, rd, tNormal, albedo, tMax);
	if(t > 0.0)
	{
		vec3 secondPoint = ro + t*rd+0.001*tNormal;
		directLight = directContrib(secondPoint, tNormal, noise);
	}
	else
	{
		
	}
	return directLight;
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

	pixel.xyz = indirectContrib(ro, normal);
	pixel.w = 1.0;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}