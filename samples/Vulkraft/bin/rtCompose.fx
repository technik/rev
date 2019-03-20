#define GBUFFER
layout(location = 3) uniform sampler2D uGBuffer;
#include "rt_common.fx"

layout(location = 4) uniform sampler2D uDirectLight;
layout(location = 5) uniform sampler2D uIndirectLight;
layout(location = 6) uniform sampler2D uDirectTaaSrc;
layout(location = 7) uniform mat4 uOldView;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;
layout(rgba32f, binding = 1) uniform image2D direct_taa;

vec3 computeWorldPos(mat4 view, ivec2 pixel_coords, float t)
{
	vec2 xy = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec4 ro = view * vec4(0,0,0,1.0);
	vec4 ssRd = vec4(normalize(vec3(xy.x, xy.y, -2.0)), 0.0);
	vec4 rd = view * ssRd;
	return (ro + t * rd).xyz;
}

ivec2 pixelCoordFromWorldPos(mat4 view, vec3 worldPos)
{
	vec4 ssRo = vec4(0,0,0,1.0);
	vec4 ssRd = inverse(view) * vec4(worldPos,1.0) - ssRo;
	vec2 ssXY = ssRd.xy * (-2.0/ssRd.z);
	vec2 pixel = (ssXY + vec2(uWindow.x/uWindow.y, 1)) * uWindow.y/2.0;
	return ivec2(pixel.x+0.5, pixel.y+0.5);
}

float oldT(ivec2 pixel)
{
	return texelFetch(uDirectTaaSrc, pixel, 0).w;
}

bool reuseTaa(float curT)
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec3 curPos = computeWorldPos(uView, pixel_coords, curT);
	ivec2 oldPxl = pixelCoordFromWorldPos(uOldView, curPos);
	if(oldPxl.x < 0 || oldPxl.y < 0 || oldPxl.x > uWindow.x || oldPxl.y > uWindow.y)
		return false;
	float lastT = oldT(oldPxl);
	vec3 oldPos = computeWorldPos(uOldView, oldPxl, lastT);
	vec3 distance = oldPos-curPos;
	return dot(distance, distance) < 0.001;
}

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//
	// Compute uvs
	vec2 centerUV = vec2(pixel_coords.x, pixel_coords.y) * (1/uWindow.xy);
	vec4 gBuffer = texelFetch(uGBuffer, pixel_coords, 0);
	vec3 worldNormal = gBuffer.xyz;

	float weight = 0.0;
	vec3 light = vec3(0.0);
	vec3 secondLight = vec3(0.0);

	vec2 xy = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec4 ro = uView * vec4(0,0,0,1.0);
	vec4 ssRd = vec4(normalize(vec3(xy.x, xy.y, -2.0)), 0.0);
	vec4 rd = uView * ssRd;
	vec4 localPoint = ro+gBuffer.w*rd+vec4(gBuffer.xyz,0.0)*1e-4;

	// Compute screen coordinates reprojected into prev frame
	vec4 prevRo = uOldView*vec4(0,0,0,1.0);
	vec4 prevRd = normalize(localPoint-prevRo);
	vec4 prevSSRd = inverse(uOldView)*prevRd;
	vec2 prevXY = -2.0*prevSSRd.xy/prevSSRd.z;
	prevXY = (prevXY*uWindow.y+0.5 + uWindow.xy)*0.5;

	float taaWeight = 0.9;
	// Clamp taa to window
	if(prevXY.x < 0 || prevXY.y < 0 || prevXY.x > uWindow.x || prevXY.y > uWindow.y)
		taaWeight = 0.0;
	vec4 taa = texelFetch(uDirectTaaSrc, ivec2(prevXY.x, prevXY.y), 0);
	vec3 prevLight = taa.xyz;
	float prevT = taa.w;
	if(prevT < 0.0)
		taaWeight = 0.0;
	else
	{
		vec4 reprojected = prevRo + prevT*prevRd;
		vec4 reproDist = reprojected-ro+gBuffer.w*rd;
		//if(dot(reproDist,reproDist) > 10.0)
		//	taaWeight = 0.0;
	}

	int windowSize = 1;
	//if(taaWeight < 0.5)
	//	windowSize = 11;
	const int minTap = -windowSize/2;
	const int maxTap = windowSize/2;

	if(gBuffer.w < 0.0)
	{
		imageStore(img_output, pixel_coords, vec4(skyColor(rd.xyz),1.0));
		imageStore(direct_taa, pixel_coords, vec4(vec3(0.0),-1));
		return;
	}

	vec3 albedo = fetchAlbedo(localPoint.xyz, worldNormal, gBuffer.w, 0);

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
				vec4 direct = texelFetch(uDirectLight, pixel_coords+ivec2(i,j), 0);
				if(direct.w < 0.0)
					continue;
				vec4 indirect = texelFetch(uIndirectLight, pixel_coords+ivec2(i,j), 0);
				if(indirect.w < 0.0)
					continue;
				light += direct.xyz * localWeight;
				secondLight += indirect.xyz * localWeight;
				weight += localWeight;
			}
		}
	}

	vec3 smoothLight = (light+secondLight)/weight;
	//smoothLight = (secondLight)/weight;
	//smoothLight = (light)/weight;

	vec3 denoised = mix(smoothLight, prevLight, taaWeight);
	vec3 directLight = weight > 0.0 ? albedo*denoised : vec3(0.0);

	//pixel.xyz = vec3(gBuffer.w/8.0);
	if(gBuffer.w > 0.0)
	{
		pixel.xyz = directLight.xyz;
		//pixel.xyz = 0.5*gBuffer.xyz+0.5;
		//pixel.xyz = vec3(gBuffer.w*0.1);
		if(reuseTaa(gBuffer.w))
			pixel.xyz = vec3(0.0,1.0,0.0);
		else
			pixel.xyz = vec3(1.0,0.0,0.0);
	}
	else
		pixel.xyz = skyColor(rd.xyz);
	pixel.w = 1;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
	imageStore(direct_taa, pixel_coords, vec4(denoised,gBuffer.w));
}