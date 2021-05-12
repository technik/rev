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

#include "material.glsl"
#include "pbr.glsl"

layout(location = 0) in vec4 vPxlNormal;
layout(location = 1) in vec4 vPxlWorldPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) readonly buffer _Material { PBRMaterial materials[]; };
layout(set = 0, binding = 2) uniform sampler2D iblLUT;

#include "pushConstants.glsl"

void main()
{
	vec3 normal = normalize(vPxlNormal.xyz);
	vec3 eye = normalize(inverse(frameInfo.view) * vec4(0,0,0,1) - vPxlWorldPos).xyz;
	vec3 halfV = normalize(eye + frameInfo.lightDir);

	float ndh = max(0, dot(halfV, normal));
	float ndl = max(0, dot(frameInfo.lightDir, normal));
	float ndv = max(0, dot(eye, normal));
	float hdv = max(0, dot(halfV, eye));

	PBRMaterial material = materials[0];

	if(renderFlag(RF_OVERRIDE_MATERIAL))
	{
		material.baseColor_a.xyz = frameInfo.overrideBaseColor.xyz;
		material.metalness = frameInfo.overrideMetallic;
		material.roughness = frameInfo.overrideRoughness;
	}

	vec3 specularColor = mix(vec3(0.04), material.baseColor_a.xyz, material.metalness);
	vec3 diffuseColor = material.baseColor_a.xyz * (1 - material.metalness);

	vec3 diffuseLight = diffuseColor  * ndl * frameInfo.lightColor / PI;
	vec3 specularLight = ndl * specularBRDF(specularColor, ndh, ndl, ndv, hdv, material.roughness) * frameInfo.lightColor;
	vec3 pxlColor = specularLight + diffuseLight;

	// TODO: Treat ambient light as an environment probe and maybe main light as a disk light

    outColor = vec4(pxlColor, 1.0);
}