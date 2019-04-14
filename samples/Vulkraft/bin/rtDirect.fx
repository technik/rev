layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
#define OVERRIDE_LGS

layout(location = 11) uniform sampler2D uGBuffer;
#define GBUFFER
#include "rt_common.fx"

layout(location = 12) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D direct_out;
layout(rgba32f, binding = 1) writeonly uniform image2D indirect_out;

//
vec3 sampleDirectLight(vec3 point, vec3 normal, vec4 noise, float tMax)
{
	vec3 light = vec3(0);
	// Sample sky light
	if(normal.y > 0)
	{
		vec3 rd = lambertianDirection(normal, noise.xy);
		float tSky = hit_any(point, rd, tMax);
		if(tSky < 0)
			light += skyColor(rd);
	}
	
	// Sample sun light
	vec3 sunSampleDir = sunDir + sunDiskSize * randomUnitVector(noise.zw);
	if(dot(sunDir, sunSampleDir) > 0)
	{
		float tSun = hit_any(point, sunSampleDir, tMax);
		if(tSun < 0)
			light += sunLight * dot(sunSampleDir, normal);
	}

	return light;
}

void color(vec3 ro, vec3 normal, vec4 noise, out vec4 direct, out vec4 indirect)
{
	vec3 atten = vec3(1.0);
	vec3 light = vec3(0.0);

	float tMax = 100.0;
	vec3 bouncePoint;
	vec3 bounceNormal;
	
	// Sun
	float tSun = -1.0;
	vec3 sunSampleDir = sunDir + sunDiskSize * randomUnitVector(noise.zw);
	if(dot(sunDir, sunSampleDir) > 0)
	{
		tSun = hit_any(ro, sunSampleDir, tMax);
	}

	// Sky/secondary surface
	float t = -1.0;
	vec3 rd = lambertianDirection(normal, noise.zw);
	t = hit(ro, rd, bounceNormal, min(tMax,10.0));
	if(t > 0.0)
	{
		vec3 albedo = fetchAlbedo(ro+t*rd, bounceNormal, t, 1);
		bouncePoint = ro + rd * t + 1e-5 * bounceNormal;
		// Scatter reflected light
		// Compute direct contribution
		indirect = vec4(albedo*sampleDirectLight(bouncePoint, bounceNormal, noise, tMax), 1.0);
	}
	else
	{
		// Sky
		indirect.xyz = vec3(0);
	}
	// Direct visibilities
	direct = vec4(0.0,0.0,tSun,t);

}

void main() {
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//
	vec3 ro, rd;
	worldRayFromPixel(uCamWorld, pixel_coords, ro, rd);
	vec4 gBufferData = texelFetch(uGBuffer, pixel_coords, 0);
	if(gBufferData.w < 0.0)
		return;
	vec3 normal = gBufferData.xyz;
	vec3 surfacePoint = ro + gBufferData.w * rd + 1e-5 * normal;

	// Scatter reflected light
	ivec2 noise_coord = ivec2(pixel_coords.x+uNoiseOffset.x,pixel_coords.y+uNoiseOffset.y);
	vec4 noise = texelFetch(uNoise, noise_coord%64, 0);
	vec4 indirect = vec4(vec3(0.0),1.0);
	// base pixel colour for image
	vec4 pixel = vec4(vec3(0.0),-1.0);
	color(surfacePoint, normal, noise, pixel, indirect);

	// output to a specific pixel in the image
	imageStore(direct_out, pixel_coords, pixel);
	imageStore(indirect_out, pixel_coords, indirect);
}