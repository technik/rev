layout(location = 11) uniform sampler2D uGBuffer;
#define GBUFFER
#include "rt_common.fx"

layout(location = 12) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D direct_out;
layout(rgba32f, binding = 1) writeonly uniform image2D indirect_out;

vec3 GGXLight(float ndl, float albedo, vec3 lightDir, vec3 normal, vec3 eye)
{
	vec3 H = normalize(eye+lightDir);
	return vec3(ndl);
}

//
vec3 sampleDirectLight(vec3 eye, vec3 point, vec3 normal, vec4 noise, float t, float tMax)
{
	vec3 albedo = fetchAlbedo(point, normal, t, 1);
	vec3 light = vec3(0);
	// Sample sky light
	vec3 rd = lambertianDirection(normal, noise.zw);
	vec3 hitNormal;
	vec3 hitPoint;
	float tSky = hit(point, rd, hitNormal, hitPoint, tMax);
	if(tSky < 0)
	{
		light += skyColor(rd) * albedo;
	}
	
	// Sample sun light
	vec3 sunSampleDir = normalize(mix(rd,sunDir,0.95));
	float sunCosine = max(0.0,dot(normal,sunSampleDir));
	if(sunCosine > 0)
	{
		float tSun = hit(point, mix(point,sunDir,0.95), hitNormal, hitPoint, tMax);
		if(tSun < 0)
		{
			light += albedo * sunCosine * sunLight;//sunDirect(albedo, hitNormal, eye);
		}
	}

	return light;
}

void color(vec3 ro, vec3 normal, vec4 noise, out vec4 direct, out vec4 indirect)
{
	vec3 atten = vec3(1.0);
	vec3 light = vec3(0.0);

	float tMax = 100.0;
	vec3 rd = normalize(lambertianDirection(normal, noise.xy));

	// Sun direct
	vec3 sampleSunDir = normalize(mix(rd,sunDir,0.95));
	float sunVisibility = 0.0;
	float sunDot = max(0.0,dot(normal,sampleSunDir));
	if(sunDot > 0)
	{	
		vec3 bouncePoint;
		vec3 bounceNormal;
		if(hit(ro, sampleSunDir, bounceNormal, bouncePoint, tMax) < 0)
		{
			// GGX specular model
			sunVisibility = 1.0;
		}
	}
	
	// Sky direct/ regular indirect
	vec3 bouncePoint;
	vec3 bounceNormal;
	float t = hit(ro, rd, bounceNormal, bouncePoint, min(tMax,1e5));
	float diffuseCos = dot(rd, normal);
	float skyContrib = 0;
	if(t > 0.0) // Hit geometry. No direct sky contribution, but we have indirect diffuse.
	{
		// Scatter reflected light
		indirect.xyz = sampleDirectLight(-rd, bouncePoint, bounceNormal, noise, t, tMax);
	}
	else
	{
		// Sky. No indirect
		indirect.xyz = vec3(0);
		skyContrib = 1.0;
	}
	// Direct visibilities
	direct = vec4(sunVisibility,0.0,0.0,skyContrib);

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
	vec4 indirect;
	color(surfacePoint, normal, noise, direct, indirect);

	// output to a specific pixel in the image
	imageStore(direct_out, pixel_coords, direct);
	imageStore(indirect_out, pixel_coords, indirect);
}