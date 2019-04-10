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
layout(rgba32f, binding = 1) writeonly uniform image2D direct_taa;
layout(rgba32f, binding = 2) writeonly uniform image2D indirect_taa;

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

bool reuseTaa(float curT, out vec4 taa, out vec4 indirectTaa)
{
	//return false;
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec3 curPos = worldPosFromPixel(uViewMtx, pixel_coords, curT);
	
	vec2 oldSS = ssFromWorldPos(uOldView, curPos);

	if(oldSS.x <= -1 || oldSS.y <= -1 || oldSS.x >= 1.0 || oldSS.y >= 1.0)
		return false;

	ivec2 oldPxl = pixelFromUV(oldSS*0.5+0.5);
	taa = texelFetch(uDirectTaaSrc, oldPxl, 0);
	float lastT = taa.w;
	if(lastT < 0.0)
		return false;
	indirectTaa = texelFetch(uIndirectTaaSrc, oldPxl, 0);
	vec3 oldPos = worldPosFromPixel(uOldView, oldPxl, lastT);
	vec3 distance = oldPos-curPos;
	float error = sqrt(dot(distance, distance))/curT;
	return error < 0.009;
}

vec3 ssReprojError(float curT)
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec3 curPos = worldPosFromPixel(uViewMtx, pixel_coords, curT);
	vec2 reconstUV = uvFromWorldPos(uViewMtx, curPos);
	pixelFromUV(reconstUV);
	//return vec3((reconstUV*uWindow.xy-0.5)-pixel_coords, 0.0);
	return vec3(pixelFromUV(reconstUV)-pixel_coords, 0.0);
}

vec3 reprojError(float curT)
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec3 curPos = worldPosFromPixel(uViewMtx, pixel_coords, curT);
	
	vec2 oldSS = ssFromWorldPos(uOldView, curPos);

	if(oldSS.x <= -1 || oldSS.y <= -1 || oldSS.x >= 1.0 || oldSS.y >= 1.0)
		return vec3(0);
	ivec2 oldPxl = pixelFromUV(oldSS*0.5+0.5);
	vec4 taa = texelFetch(uDirectTaaSrc, oldPxl, 0);
	float lastT = taa.w;
	if(lastT < 0.0)
		return vec3(0);

	//vec3 oldPos = worldPosFromSS(uOldView, oldSS, lastT);
	vec3 oldPos = worldPosFromPixel(uOldView, oldPxl, lastT);
	vec3 distance = oldPos-curPos;
	return vec3(abs(distance)/(curT*0.1));
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
	vec4 rd = vec4(worldSpaceRay(uCamWorld,2*pixelUVs-1), 0.0);
	if(gBuffer.w < 0.0)
	{
		imageStore(img_output, pixel_coords, vec4(skyColor(rd.xyz),1.0));
		imageStore(direct_taa, pixel_coords, vec4(vec3(0.0),-1));
		imageStore(indirect_taa, pixel_coords, vec4(vec3(0.0),-1));
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
	int windowSize = 1;//9;
	vec4 taa = vec4(0);
	vec4 indirectTaa = vec4(0);
	float taaWeight = 0.0;
	if(reuseTaa(gBuffer.w, taa, indirectTaa))
	{
		windowSize = 1;
		taaWeight = taa.y / (taa.y + windowSize*windowSize);
		taa.y = min(3.0, taa.y+windowSize*windowSize);
	}
	else
	{
		taa.y = windowSize*windowSize;
		indirectTaa = vec4(0.0);
	}
	taa.w = gBuffer.w;

	const int minTap = -windowSize/2;
	const int maxTap = windowSize/2;

	float kernelWeight = 1.0;
	/*for(int i = minTap; i <= maxTap; ++i)
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
					visibility += localWeight;
				}
				if(direct.z < 0.0)
				{
					sunVisibility += localWeight;
				}
				secondLight += localWeight*texelFetch(uIndirectLight, pixel_coords+ivec2(i,j), 0).xyz;
			}
		}
	}*/
	
	visibility = mix(visibility/kernelWeight, max(0.0,taa.x), taaWeight);
	sunVisibility = mix(sunVisibility/kernelWeight, max(0.0,taa.z), taaWeight);
	secondLight = mix(secondLight/kernelWeight, indirectTaa.xyz, taaWeight);
	taa.x = visibility;
	taa.z = sunVisibility;

	// base pixel colour for image
	vec4 pixel = vec4(0.0);

	vec3 smoothLight = visibility * irradiance(gBuffer.xyz) + sunVisibility * sunLight * dot(sunDir,gBuffer.xyz);
	vec4 localPoint = ro+rd*gBuffer.w-gBuffer*0.1;
	vec3 albedo = fetchAlbedo(localPoint.xyz, worldNormal, gBuffer.w, 0);
	pixel.xyz = albedo*(smoothLight+secondLight.xyz);

	//pixel.xyz = indirectTaa.xyz;
	//pixel.xyz = vec3(smoothLight);
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
	imageStore(indirect_taa, pixel_coords, vec4(secondLight,0.0));
}