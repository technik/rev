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
layout(set = 0, binding = 3) uniform sampler2D textures[];

#include "pushConstants.glsl"

vec3 directLight(
	vec3 specularColor,
	vec3 diffuseColor,
	vec3 lightColor,
	float roughness,
	float ndh,
	float ndl,
	float ndv,
	float hdv)
{
	float r = max(1e-4, roughness);
	// Single bounce specular lighting0
	vec3 specular = specularBRDF(specularColor, ndh, ndl, ndv, hdv, r);

	// Energy compensation - substract single bounce specular from the light that goes into diffuse
	vec2 reflectedLight = textureLod(iblLUT, vec2(ndl, roughness), 0).xy;
	vec3 totalFresnel = specularColor * reflectedLight.x + reflectedLight.y;

	vec3 kD = diffuseColor * (1-totalFresnel);
	vec3 diffuse = kD / PI;
	return (specular + diffuse) * ndl * lightColor;
}

vec3 envLight(
	vec3 specularColor,
	vec3 diffuseColor,
	vec3 lightColor,
	float roughness,
	float ndh,
	float ndl,
	float ndv,
	float hdv)
{
	vec2 iblFresnel = textureLod(iblLUT, vec2(ndv, roughness), 0).xy;
	vec3 specular = specularColor * iblFresnel.x + iblFresnel.y;
	vec3 diffuse = (1 - specular) * diffuseColor / PI;
	return (diffuse + specular) * lightColor;
}

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

	vec3 mainLight = directLight(
		specularColor,
		diffuseColor,
		frameInfo.lightColor,
		material.roughness,
		ndh, ndl, ndv, hdv);

	vec3 ambientLight = envLight(
		specularColor,
		diffuseColor,
		frameInfo.ambientColor,
		material.roughness,
		ndh, ndl, ndv, hdv);
	
	vec3 pxlColor = mainLight + ambientLight;

	// TODO: Treat ambient light as an environment probe and maybe main light as a disk light

	// Basic toneMapping
	vec3 toneMapped = pxlColor / (1 + pxlColor);

    outColor = vec4(toneMapped, 1.0);
}