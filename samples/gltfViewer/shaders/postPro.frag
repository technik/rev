//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "material.glsl"
#include "pbr.glsl"

layout(location = 0) in vec2 vPxlTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0, rgba32f) uniform image2D HDRLight;

layout(push_constant,scalar) uniform Constants
{
	uint renderFlags;
	vec3 ambientColor;
	vec2 windowSize;
	float exposure;
	float bloom;
} pushC;

void main()
{
	ivec2 pixelPos = ivec2(
		pushC.windowSize.x * vPxlTexCoord.x,
		pushC.windowSize.y * vPxlTexCoord.y);

	// Gather slight bloom
	vec4 hdrColor = vec4(0);
	int bloomWin = 5;
	ivec2 minPos = max(pixelPos-bloomWin, ivec2(0));
	ivec2 maxPos = min(pixelPos+bloomWin, ivec2(pushC.windowSize-1));

	//if(bloomWin == 0)
	{
		vec4 bloomColor = imageLoad(HDRLight, pixelPos);
		if(bloomColor.w < 0)
		{
			bloomColor = vec4(pushC.ambientColor,1);
		}
		hdrColor += bloomColor;
	}
	if(bloomWin > 0)
	{
		for(int j = minPos.y; j <= maxPos.y; ++j)
		{
			for(int i = minPos.x; i <= maxPos.x; ++i)
			{
				if(ivec2(i,j) == pixelPos)
					continue;
				float distance = 100 * (float(abs(i-pixelPos.x) * abs(j-pixelPos.y))) / pushC.windowSize.y;
				float weight = exp(-distance);

				vec4 bloomColor = imageLoad(HDRLight, ivec2(i, j));
				if(bloomColor.w < 0)
				{
					bloomColor = vec4(pushC.ambientColor,1);
				}
				vec4 delta = weight * pow(bloomColor, vec4(2));
				//delta = pow(delta, vec4(2)); 
				hdrColor += 0.001 * delta;
			}
		}
	}
	hdrColor *= pushC.exposure;
	vec4 toneMapped = hdrColor / (1 + hdrColor);
	outColor = toneMapped;
}