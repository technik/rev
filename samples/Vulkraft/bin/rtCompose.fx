#include "rt_common.fx"

layout(location = 3) uniform sampler2D uGBuffer;
layout(location = 4) uniform sampler2D uDirectLight;
layout(location = 5) uniform sampler2D uIndirectLight;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

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
	vec3 localPoint = ro+gBuffer.w*rd+gBuffer.xyz*1e-4;
	vec3 albedo = (localPoint.y > 0.85) ? vec3(0.35, 0.5, 0.35) : vec3(0.85, 0.5, 0.4);

	for(int i = minTap; i <= maxTap; ++i)
	{
		for(int j = minTap; j <= maxTap; ++j)
		{
			vec2 uvs = vec2(pixel_coords.x+i, pixel_coords.y+j) * (1/uWindow.xy);
			vec4 pointGBuffer = textureLod(uGBuffer, uvs, 0);
			if(pointGBuffer.w > 0.0)
			{
				float localWeight = max(0.0, dot(pointGBuffer.xyz, gBuffer.xyz));
				//localWeight *= sqrt(0.5)*windowSize-sqrt(float(i*i+j*j));
				vec4 direct = textureLod(uDirectLight, uvs, 0);
				if(direct.w < 0.0)
					continue;
				light += direct.xyz * localWeight;
				vec4 indirect = textureLod(uIndirectLight, uvs, 0);
				if(direct.w < 0.0)
					continue;
				secondLight += indirect.xyz * localWeight;
				weight += localWeight;
			}
		}
	}
	vec3 directLight = weight > 0.0 ? albedo*(light+secondLight)/weight : vec3(0.0);

	//pixel.xyz = vec3(gBuffer.w/8.0);
	if(gBuffer.w > 0.0)
		pixel.xyz = directLight.xyz;
	else
		pixel.xyz = skyColor(rd);
	pixel.w = 1;

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}