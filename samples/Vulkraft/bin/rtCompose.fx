#include "rt_common.fx"

layout(location = 3) uniform sampler2D uGBuffer;
layout(location = 4) uniform sampler2D uDirectLight;
layout(location = 5) uniform sampler2D uIndirectLight;
layout(location = 6) uniform sampler2D uDirectTaaSrc;
layout(location = 7) uniform mat4 uOldView;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;
layout(rgba32f, binding = 1) uniform image2D direct_taa;

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//
	// Compute uvs
	vec2 centerUV = vec2(pixel_coords.x, pixel_coords.y) * (1/uWindow.xy);
	vec4 gBuffer = textureLod(uGBuffer, centerUV, 0);

	float weight = 0.0;
	vec3 light = vec3(0.0);
	vec3 secondLight = vec3(0.0);

	const int windowSize = 5;
	const int minTap = -windowSize/2;
	const int maxTap = windowSize/2;

	vec2 xy = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 ro = (uView * vec4(0,0,0,1.0)).xyz;
	vec3 rd = (uView * vec4(normalize(vec3(xy.x, xy.y, -2.0)), 0.0)).xyz;
	if(gBuffer.w < 0.0)
	{
		imageStore(img_output, pixel_coords, vec4(skyColor(rd),1.0));
		return;
	}
	vec3 localPoint = ro+gBuffer.w*rd+gBuffer.xyz*1e-4;
	vec3 green = vec3(0.00, 0.5, 0.15);
	vec3 albedo = (localPoint.y > 0.85) ? green : vec3(0.63, 0.42, 0.2717);
	albedo *= ((int(localPoint.x*8)%2)^(int(localPoint.z*8)%2)^(int(localPoint.y*8)%2))*0.1+0.9;

	for(int i = minTap; i <= maxTap; ++i)
	{
		for(int j = minTap; j <= maxTap; ++j)
		{
			vec2 uvs = vec2(pixel_coords.x+i, pixel_coords.y+j) * (1/uWindow.xy);
			vec4 pointGBuffer = textureLod(uGBuffer, uvs, 0);
			if(pointGBuffer.w > 0.0)
			{
				float localWeight = max(0.0, dot(pointGBuffer.xyz, gBuffer.xyz));
				localWeight *= sqrt(0.5)*windowSize-sqrt(float(i*i+j*j));
				localWeight *= max(0.0,0.2-abs(pointGBuffer.w-gBuffer.w));
				vec4 direct = textureLod(uDirectLight, uvs, 0);
				if(direct.w < 0.0)
					continue;
				vec4 indirect = textureLod(uIndirectLight, uvs, 0);
				if(indirect.w < 0.0)
					continue;
				light += direct.xyz * localWeight;
				secondLight += indirect.xyz * localWeight;
				weight += localWeight;
			}
		}
	}

	vec3 smoothLight = light/weight;

	// Compute screen coordinates reprojected into prev frame
	vec4 prevPoint = uOldView*inverse(uView)*vec4(localPoint,1.0);
	vec4 prevRd = inverse(uOldView)*prevPoint-vec4(0,0,0,1.0);
	vec2 prevXY = -2.0*prevRd.xy/prevRd.z;
	prevXY = (prevXY*uWindow.y+0.5 + uWindow.xy)*0.5;

	vec3 prevLight = texelFetch(uDirectTaaSrc,
						 ivec2(prevXY.x, prevXY.y)
						 , 0).xyz;
	vec3 denoisedDirect = mix(prevLight, smoothLight, 0.1);


	vec3 directLight = weight > 0.0 ? albedo*denoisedDirect : vec3(0.0);

	//pixel.xyz = vec3(gBuffer.w/8.0);
	if(gBuffer.w > 0.0)
		pixel.xyz = directLight.xyz;
		//pixel.xyz = vec3(prevX/uWindow.x);
	else
		pixel.xyz = skyColor(rd);
	pixel.w = 1;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
	imageStore(direct_taa, pixel_coords, vec4(denoisedDirect,1.0));
}