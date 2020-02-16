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
	vec3 rd = lambertianDirection(normal, noise.zw);
	vec3 hitNormal;
	vec3 hitPoint;
	float tSky = hit(point, rd, hitNormal, hitPoint, tMax);
	if(tSky < 0)
		light += skyColor(rd);
	
	// Sample sun light
	vec3 sunSampleDir = normalize(mix(rd,sunDir,0.95));
	float sunCosine = max(0.0,dot(normal,sunSampleDir));
	if(sunCosine > 0)
	{
		float tSun = hit(point, mix(point,sunDir,0.95), hitNormal, hitPoint, tMax);
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
	vec3 rd = normalize(lambertianDirection(normal, noise.xy));

	// Sun direct
	vec3 sampleSunDir = normalize(mix(rd,sunDir,0.95));
	float sunContrib = 0.0;
	float sunDot = max(0.0,dot(normal,sampleSunDir));
	if(sunDot > 0)
	{	
		vec3 bouncePoint;
		vec3 bounceNormal;
		if(hit(ro, sampleSunDir, bounceNormal, bouncePoint, tMax) < 0)
			sunContrib = sunDot;
	}
	
	// Sky direct/ regular indirect
	vec3 bouncePoint;
	vec3 bounceNormal;
	float t = hit(ro, rd, bounceNormal, bouncePoint, min(tMax,1e5));
	float diffuseCos = dot(rd, normal);
	float skyContrib = 0;
	if(t > 0.0) // Hit geometry. No direct sky contribution, but we have indirect diffuse.
	{
		vec3 secondaryAlbedo = fetchAlbedo(bouncePoint, bounceNormal, t, 1);
		// Scatter reflected light
		vec3 indirectLight = sampleDirectLight(bouncePoint, bounceNormal, noise, tMax);
		// Compute direct contribution
		bouncePoint += 1e-5*bounceNormal;
		indirect.xyz = secondaryAlbedo * indirectLight;
	}
	else
	{
		// Sky. No indirect
		indirect.xyz = vec3(0);
		skyContrib = 1.0;
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
	vec4 indirect;
	color(surfacePoint, normal, noise, direct, indirect);

	// output to a specific pixel in the image
	imageStore(direct_out, pixel_coords, direct);
	imageStore(indirect_out, pixel_coords, indirect);
}