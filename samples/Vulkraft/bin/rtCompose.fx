#define GBUFFER
layout(location = 11) uniform sampler2D uGBuffer;
#include "rt_common.fx"

layout(location = 12) uniform sampler2D uDirectLight;
layout(location = 13) uniform sampler2D uIndirectLight;
layout(location = 16) uniform mat4 uOldView;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

vec2 ssFromWorldPos(mat4 view, vec3 worldPos)
{
	vec4 ssPos = uProj * view * vec4(worldPos, 1.0);
	return ssPos.xy / ssPos.w;
}

vec2 uvFromWorldPos(mat4 view, vec3 worldPos)
{
	return ssFromWorldPos(view,worldPos) * 0.5+0.5;
}

vec3 worldPosFromSS(mat4 view, vec2 ssPos, float t)
{
	vec4 posB = vec4(ssPos.x, ssPos.y, 1.0, 1.0);

	mat4 invProj = inverse(uProj); 
	mat4 camWorld = inverse(view);
	vec4 wsDir = vec4(worldSpaceRay(camWorld, ssPos), 0.0);
	vec4 wsPos = t * wsDir + camWorld * vec4(0,0,0,1);
	return wsPos.xyz;
}

vec3 worldPosFromUV(mat4 view, vec2 uvs, float t)
{
	return worldPosFromSS(view, uvs * 2 - 1, t);
}

vec3 worldPosFromPixel(mat4 view, ivec2 pixel_coords, float t)
{
	vec2 uvs = vec2(pixel_coords.x+0.5, pixel_coords.y+0.5) / uWindow.xy;
	return worldPosFromUV(view, uvs, t);
}

ivec2 pixelFromUV(vec2 uv)
{
	return ivec2(uv*uWindow.xy);
}

vec3 irradiance(in vec3 dir)
{
	return skyColor(normalize(dir + vec3(0.0,1.0,0.0)));
}

void main() {
	// Read GBuffer and early out on sky
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 gBuffer = texelFetch(uGBuffer, pixel_coords, 0);
	vec2 pixelUVs = vec2(pixel_coords.x+0.5, pixel_coords.y+0.5) * (1/uWindow.xy);
	
	vec4 vsRd = viewSpaceRay(2*pixelUVs-1);
	vec4 rd = uCamWorld * vsRd;
	
	float d = gBuffer.w;
	if(d < 0.0)
	{
		imageStore(img_output, pixel_coords, vec4(skyColor(rd.xyz),1.0));
		return;
	}
	float t = -d/vsRd.z;
	//
	vec3 worldNormal = gBuffer.xyz;
	vec4 ro = uCamWorld * vec4(0,0,0,1.0);

	vec4 directBuffer = texelFetch(uDirectLight, pixel_coords, 0);
	float skyVisibility = directBuffer.w;
	float sunVisibility = directBuffer.x;
	vec3 secondLight = texelFetch(uIndirectLight, pixel_coords, 0).xyz;

	// base pixel colour for image
	vec4 pixel = vec4(0.0);

	vec3 smoothSkyLight = vec3(skyVisibility) * irradiance(worldNormal);
	vec4 localPoint = ro+rd*t;
	vec3 albedo = fetchAlbedo(localPoint.xyz, worldNormal, t, 0);
	// Sun GGX
	vec3 eye = -rd.xyz;
	vec3 sunBrdf = sunDirect(albedo, worldNormal, eye);
	vec3 sunContrib = sunVisibility * sunBrdf;
	
	pixel.xyz = sunContrib+albedo*(smoothSkyLight + secondLight.xyz);
	//pixel.xyz = smoothSkyLight.xyz;
	pixel.w = 1;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}