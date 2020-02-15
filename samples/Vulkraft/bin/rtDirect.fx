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
	float skyCosine = max(0.0, dot(rd,normal));
	vec3 hitNormal;
	float tSky = hit(point, rd, hitNormal, tMax);
	if(tSky < 0)
		light += skyCosine * skyColor(rd);
	
	// Sample sun light
	vec3 sunSampleDir = normalize(mix(rd,sunDir,0.95));
	float sunCosine = max(0.0,dot(normal,sunSampleDir));
	if(sunCosine > 0)
	{
		float tSun = hit(point, mix(point,sunDir,0.95), hitNormal, tMax);
		if(tSun < 0)
			light += sunLight * sunCosine;
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

	// Sun direct
	vec3 sampleSunDir = normalize(mix(rd,sunDir,0.95));
	float sunContrib = 0.0;
	float sunDot = dot(normal,sampleSunDir);
	if(sunDot > 0)
	{	
		if(hit(ro, sampleSunDir, bounceNormal, tMax) < 0)
			sunContrib = sunDot;
	}
	
	// Sky direct/ regular indirect
	float t = hit(ro, rd, bounceNormal, min(tMax,10.0));
	float diffuseCos = dot(normalize(rd), normal);
	float skyContrib = 0;
	if(t > 0.0) // Hit geometry. No direct sky contribution, but we have indirect diffuse.
	{
		vec3 secondaryAlbedo = vec3(1.0);//fetchAlbedo(ro+t*rd, bounceNormal, t, 1);
		bouncePoint = ro + rd * t + 0.00001 * bounceNormal;
		// Scatter reflected light
		// Compute direct contribution
		indirect = vec4(diffuseCos * secondaryAlbedo * sampleDirectLight(bouncePoint, bounceNormal, noise, tMax), 1.0);
	}
	else
	{
		// Sky. No indirect
		skyContrib = diffuseCos;
		indirect.xyz = vec3(0);
	}
	// Direct visibilities
	direct = vec4(0.0,0.0,sunContrib,skyContrib);

}

void main() {
	// base pixel colour for image
	vec4 direct = vec4(vec3(0.0),-1.0);
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
	color(surfacePoint, normal, noise, direct, indirect);

	// output to a specific pixel in the image
	imageStore(direct_out, pixel_coords, direct);
	imageStore(indirect_out, pixel_coords, indirect);
}