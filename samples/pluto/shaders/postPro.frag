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

layout(set = 0, binding = 0, rgba32f) readonly uniform image2D HDRLight;

layout(push_constant,scalar) uniform Constants
{
	vec3 ambientColor;
	uint renderFlags;
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
	vec4 hdrColor = imageLoad(HDRLight, pixelPos);
	
	// Exposure
	hdrColor *= pushC.exposure;

	// Tone mapping
	vec4 toneMapped = hdrColor / (1 + hdrColor);
	toneMapped.a = 1;
	outColor = toneMapped;
}