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
	vec3 rd = lambertianDirection(normal, noise.xy);
	vec3 hitNormal;
	float tSky = hit(point, rd, hitNormal, tMax);
	if(tSky < 0)
		light += skyColor(rd);
	
	// Sample sun light
	vec3 sunSampleDir = mix(rd,sunDir,0.95);
	if(dot(sunDir, sunSampleDir) > 0)
	{
		float tSun = hit(point, mix(point,sunDir,0.95), hitNormal, tMax);
		if(tSun < 0)
			light += sunLight * dot(sunDir, sunSampleDir);
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
	vec3 rd = lambertianDirection(normal, noise.zw);
	
	float tSun = hit(ro, mix(rd,sunDir,0.95), bounceNormal, tMax);
	float t = hit(ro, rd, bounceNormal, min(tMax,10.0));
	if(t > 0.0)
	{
		vec3 albedo = fetchAlbedo(ro+t*rd, bounceNormal, t, 1);
		bouncePoint = ro + rd * t + 0.00001 * bounceNormal;
		// Scatter reflected light
		// Compute direct contribution
		indirect = vec4(sampleDirectLight(bouncePoint, bounceNormal, noise, tMax), 1.0);
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
	// base pixel colour for image
	vec4 pixel = vec4(vec3(0.0),-1.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//
	// Compute uvs
	vec2 pixelUVs = vec2(pixel_coords.x, pixel_coords.y) * (1/uWindow.xy);
	vec3 ro = (uCamWorld * vec4(0,0,0,1.0)).xyz;
	vec3 rd = worldSpaceRay(uCamWorld, 2*pixelUVs-1);
	vec4 gBufferData = texelFetch(uGBuffer, pixel_coords, 0);
	if(gBufferData.w < 0.0)
		return;
	vec3 normal = gBufferData.xyz;
	vec3 surfacePoint = ro + gBufferData.w * rd + 1e-5 * normal;

	// Scatter reflected light
	ivec2 noise_coord = ivec2(pixel_coords.x+uNoiseOffset.x,pixel_coords.y+uNoiseOffset.y);
	vec4 noise = texelFetch(uNoise, noise_coord%64, 0);
	vec4 indirect = vec4(vec3(0.0),1.0);
	color(surfacePoint, normal, noise, pixel, indirect);

	// output to a specific pixel in the image
	imageStore(direct_out, pixel_coords, pixel);
	imageStore(indirect_out, pixel_coords, indirect);
}