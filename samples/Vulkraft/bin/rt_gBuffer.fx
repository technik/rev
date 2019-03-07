#include "rt_common.fx"


void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0,0.0,0.0,-1.0); // (Normal.xyz, t)
	
	float noiseX = float((gl_GlobalInvocationID.x) % 64) / 64;
	float noiseY = float((gl_GlobalInvocationID.y) % 64) / 64;
	vec4 noise = texture(uNoise, vec2(noiseX, noiseY));
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);// + noise.xy;
	//
	// Compute uvs
	vec2 uvs = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 rd = (uView * vec4(normalize(vec3(uvs.x, uvs.y, -2.0)), 0.0)).xyz;
	vec3 ro = (uView * vec4(0,0,0,1.0)).xyz;

	vec3 normal;
	vec3 albedo;
	float tMax = 1000.0;
	float t = hit(ro, rd, normal, albedo, tMax);
	if(t >= 0)
	{
		pixel.xyz = vec3(1.0,0.0,0.0);//normal;
		pixel.w = t;
	}
  
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}