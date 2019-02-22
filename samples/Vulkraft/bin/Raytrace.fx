#include "rt_common.fx"

layout(location = 3) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

vec3 color(vec3 ro, vec3 rd, out float tOut)
{
	vec3 normal;
	vec3 atten = vec3(1.0);
	vec3 light = vec3(0.0);
	float tMax = 100.0;
	tOut = tMax;
	for(int i = 0; i < 4; ++i)
	{
		vec3 albedo;
		float t = hit(ro, rd, normal, albedo, tMax);
		if(t > 0.0)
		{
			if(i == 0)
				tOut = t;
			atten *= 0.5 * albedo;
			ro = ro + rd * t + 0.0001 * normal;
			// Scatter reflected light
			float noiseX = float((gl_GlobalInvocationID.x + i)) / 64;
			float noiseY = float((gl_GlobalInvocationID.y + (i>>1))) / 64;
			vec4 noise = textureLod(uNoise, vec2(noiseX, noiseY), 0);

			// Compute direct contribution
			vec3 skyDir = randomUnitVector(noise.xy);
			if(dot(skyDir,normal) < 0)
				skyDir = -skyDir;
			vec3 skyNorm, skyAlbedo;
			if(hit(ro, skyDir, skyNorm, skyAlbedo, tMax) < 0)
				light += atten * skyColor(rd);

			// Indirect contribution
			if(noise.y > 0.15)
			{
				rd = lambertianDirection(normal, noise.zw);
			}
			else
				rd = reflect(rd, normal);
		}
		else
		{
			// Sky
			light += atten * skyColor(rd);
			break;
		}
	}
	return light;
}

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0);
	
	float noiseX = float((gl_GlobalInvocationID.x) % 64) / 64;
	float noiseY = float((gl_GlobalInvocationID.y) % 64) / 64;
	vec4 noise = texture(uNoise, vec2(noiseX, noiseY));
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);// + noise.xy;
	//
	// Compute uvs
	vec2 uvs = (vec2(pixel_coords.x, pixel_coords.y)+noise.xy) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 ro = (uView * vec4(0,0,0,1.0)).xyz;
	vec3 rd = (uView * vec4(normalize(vec3(uvs.x, uvs.y, -2.0)), 0.0)).xyz;

	float t;
	pixel.xyz = color(ro, rd, t);
	pixel.w = t;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}