#define GBUFFER
layout(location = 11) uniform sampler2D uGBuffer;
#include "rt_common.fx"

layout(location = 12) uniform sampler2D uDirectLight;
layout(location = 13) uniform sampler2D uIndirectLight;
layout(location = 14) uniform sampler2D uDirectTaaSrc;
layout(location = 15) uniform sampler2D uIndirectTaaSrc;
layout(location = 16) uniform mat4 uOldView;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;
layout(rgba32f, binding = 1) uniform image2D direct_taa;
layout(rgba32f, binding = 2) uniform image2D indirect_taa;

ivec2 pixelCoordFromWorldPos(mat4 view, vec3 worldPos)
{
	vec4 ssPos = uProj * view * vec4(worldPos, 1.0);
	vec2 ssXY = ssPos.xy / ssPos.w;
	vec2 pixel = (ssXY * 0.5 + 0.5) * uWindow.xy;
	return ivec2(pixel.x-0.5, pixel.y-0.5);
}

vec2 ssUvFromWorldPos(mat4 view, vec3 worldPos)
{
	vec4 ssPos = uProj * view * vec4(worldPos, 1.0);
	vec2 ssXY = ssPos.xy / ssPos.w;
	return ssXY * 0.5+0.5;
}

vec3 worldPosFromUV(mat4 view, vec2 uvs, float t)
{
	vec2 csPos = uvs * 2 - 1;
	vec4 posB = vec4(csPos.x, csPos.y, 1.0, 1.0);

	mat4 invProj = inverse(uProj); 
	vec4 vsDir = normalize(invProj * posB);
	vec4 wsPos = inverse(view) * vec4((t*vsDir).xyz,1.0);
	return wsPos.xyz;
}

vec3 worldPosFromPixel(mat4 view, ivec2 pixel_coords, float t)
{
	vec2 uvs = vec2(pixel_coords.x+0.5, pixel_coords.y+0.5) /uWindow.xy;
	return worldPosFromUV(view, uvs, t);
}

bool reuseTaa(float curT, out vec4 taa)
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec3 curPos = worldPosFromPixel(uViewMtx, pixel_coords, curT);
	vec2 oldUV = ssUvFromWorldPos(uOldView, curPos);

	if(oldUV.x < 0 || oldUV.y < 0 || oldUV.x >= 1.0 || oldUV.y >= 1.0)
		return false;
	taa = texelFetch(uDirectTaaSrc, ivec2(oldUV*uWindow.xy), 0);
	float lastT = taa.w;
	if(lastT < 0.0)
		return false;
	vec3 oldPos = worldPosFromUV(uOldView, oldUV, lastT);
	vec3 distance = oldPos-curPos;
	float error = abs(dot(distance, distance)/curT);
	return error < 0.1;
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
	vec4 rd = vec4(worldSpaceRay(2*pixelUVs-1), 0.0);
	if(gBuffer.w < 0.0)
	{
		imageStore(img_output, pixel_coords, vec4(skyColor(rd.xyz),1.0));
		imageStore(direct_taa, pixel_coords, vec4(vec3(0.0),-1));
		return;
	}
	//
	vec3 worldNormal = gBuffer.xyz;
	vec4 ro = uCamWorld * vec4(0,0,0,1.0);

	vec4 directBuffer = texelFetch(uDirectLight, pixel_coords, 0);
	float visibility = directBuffer.w < 0.0 ? 1.0 : 0.0;
	float sunVisibility = directBuffer.z < 0.0 ? 1.0 : 0.0;
	vec3 secondLight = texelFetch(uIndirectLight, pixel_coords, 0).xyz;

	// Temporal denoising
	int windowSize = 5;
	vec4 taa = vec4(0.0,0.0,0.0,0.0);
	float taaWeight = 0.0;
	if(reuseTaa(gBuffer.w, taa))
	{
		windowSize = 1;
		taaWeight = taa.y / (taa.y + 1.0);
		taa.y = min(15.0, taa.y+1.0);
	}
	else
	{
		taa.y = windowSize*windowSize;
	}
	taa.w = gBuffer.w;

	//if(taaWeight < 0.5)
	//	windowSize = 11;
	const int minTap = -windowSize/2;
	const int maxTap = windowSize/2;

	float kernelWeight = 1.0;
	for(int i = minTap; i <= maxTap; ++i)
	{
		for(int j = minTap; j <= maxTap; ++j)
		{
			vec2 uvs = vec2(pixel_coords.x+i, pixel_coords.y+j) * (1/uWindow.xy);
			vec4 pointGBuffer = texelFetch(uGBuffer, pixel_coords+ivec2(i,j), 0);
			if(pointGBuffer.w > 0.0)
			{
				float localWeight = max(0.0, dot(pointGBuffer.xyz, gBuffer.xyz));
				localWeight *= sqrt(0.5)*windowSize-sqrt(float(i*i+j*j));
				localWeight *= max(0.0,0.2-abs(pointGBuffer.w-gBuffer.w));
				kernelWeight += localWeight;
				vec4 direct = texelFetch(uDirectLight, pixel_coords+ivec2(i,j), 0);
				if(direct.w < 0.0)
				{
					visibility += kernelWeight;
				}
				if(direct.z < 0.0)
				{
					sunVisibility += kernelWeight;
				}
			}
		}
	}
	
	visibility = mix(visibility/kernelWeight, max(0.0,taa.x), taaWeight);
	sunVisibility = mix(sunVisibility/kernelWeight, taa.z, taaWeight);
	taa.x = visibility;
	taa.z = sunVisibility;

	// base pixel colour for image
	vec4 pixel = vec4(0.0);

	vec3 smoothLight = visibility * irradiance(gBuffer.xyz) + 0.50 * sunVisibility;
	vec4 localPoint = ro+rd*gBuffer.w-gBuffer*0.1;
	vec3 albedo = fetchAlbedo(localPoint.xyz, worldNormal, gBuffer.w, 0);
	pixel.xyz = albedo*(smoothLight+secondLight);

	//pixel.xyz = vec3(sunVisibility);
	//pixel.xyz = vec3(visibility);
	//pixel.xyz = 0.5*gBuffer.xyz+0.5;
	//pixel.xyz = vec3(gBuffer.w*0.1);
	//if(reuseTaa(gBuffer.w))
	//	pixel.xyz = vec3(0.0,1.0,0.0);
	//else
	//	pixel.xyz = vec3(1.0,0.0,0.0);
	pixel.w = 1;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
	imageStore(direct_taa, pixel_coords, taa);
}