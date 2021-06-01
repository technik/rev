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

layout(location = 0) in vec4 vPxlNormal;
layout(location = 1) in vec4 vPxlWorldPos;
layout(location = 2) in vec4 vPxlWorldTan;
layout(location = 3) in vec2 vPxlTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) readonly buffer _Material { PBRMaterial materials[]; };
layout(set = 0, binding = 2) uniform sampler2D iblLUT;
layout(set = 0, binding = 3) uniform sampler2D textures[];

#include "pushConstants.glsl"

vec2 Eavg(float ndl, float r)
{
	return textureLod(iblLUT, vec2(ndl, r), 0).xy;
}

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
	vec3 specularSS = specularBRDF(specularColor, ndh, ndl, ndv, hdv, r); // Single scattering

	vec2 Ei3 = textureLod(iblLUT, vec2(ndl, roughness), 0).xy;
	float Ei = Ei3.x + Ei3.y;
	//float compE = compEavg(m_roughness);
	//float Eavg = 1 - compE;
	//float den = pi_v<float> *compEavg(m_roughness);
	//float fms = (1 - Eo) * (1 - Ei) / den;

	//vec3 Favg = (20.f * m_F0 + 1.f) / 21.f;
	//vec3 Fo = Eo3.x() * m_F0 + Eo3.y();
	//vec3 Fi = Ei3.x() * m_F0 + Ei3.y();
	vec3 Fms = specularColor * (1 - Ei) / Ei;

	vec3 kD = max(vec3(0), diffuseColor - (Ei3.x * specularColor + Ei3.y));
	vec3 diffuse = kD / PI;
	return (specularSS * (1+Fms) + diffuse) * lightColor * ndl;
}

vec3 envLight(
	vec3 specularColor,
	vec3 diffuseColor,
	vec3 lightColor,
	float roughness,
	float ao,
	float ndh,
	float ndl,
	float ndv,
	float hdv)
{
	vec3 irradiance = lightColor;
	vec3 radiance = lightColor;
	
	vec2 iblFresnel = textureLod(iblLUT, vec2(ndv, roughness), 0).xy;
	vec3 FssEss = specularColor * iblFresnel.x + iblFresnel.y;

	// Multiple scattering
	float Ess = iblFresnel.x + iblFresnel.y;
	float Ems = 1-Ess;
	vec3 Favg = specularColor + (1-specularColor)/21;
	vec3 Fms = FssEss*Favg/(1-(1-Ess)*Favg);

	return (FssEss * radiance + (Fms*Ems + diffuseColor) * irradiance) *  mix(diffuseColor, vec3(1.0), ao);
}

void main()
{
	vec3 normal = normalize(vPxlNormal.xyz);
	vec3 eye = normalize(inverse(frameInfo.view) * vec4(0,0,0,1) - vPxlWorldPos).xyz;
	vec3 halfV = normalize(eye + frameInfo.lightDir);

	PBRMaterial material = materials[0];
	if(material.baseColorTexture >= 0)
	{
		uint index = material.baseColorTexture;
		material.baseColor_a *= texture(nonuniformEXT(textures[index]), vPxlTexCoord);
	}
	if(material.pbrTexture >= 0)
	{
		uint index = material.pbrTexture;
		vec2 metalRough = texture(textures[nonuniformEXT(index)], vPxlTexCoord).bg;
		material.metalness *= metalRough.x;//*metalRough.x;
		material.roughness *= metalRough.y;//*metalRough.y;
	}
	float ao = 1.0;
	if(material.aoTexture >= 0 && !renderFlag(RF_DISABLE_AO))
	{
		uint index = material.aoTexture;
		ao = texture(textures[nonuniformEXT(index)], vPxlTexCoord).x;
	}
	if(material.normalTexture >= 0 && !renderFlag(RF_NO_NORMAL_MAP))
    {
        // Tangent
        vec3 tan = vPxlWorldTan.xyz;
        vec3 wsBitangent = cross(normal, tan) * vPxlWorldTan.w;

        mat3 modelFromTangent = mat3(tan, wsBitangent, normal);

        uint txtId = material.normalTexture;
        vec3 tsNormal = texture(textures[nonuniformEXT(txtId)], vPxlTexCoord).xyz;
        tsNormal = tsNormal + vec3(0,0,1e-2);
        tsNormal = normalize(tsNormal * 255.0 - 127.0);
        normal = normalize(modelFromTangent * tsNormal);
        //material.baseColor_a.xyz = normal;
    }

	if(renderFlag(RF_OVERRIDE_MATERIAL))
	{
		material.baseColor_a.xyz = frameInfo.overrideBaseColor.xyz;
		material.metalness = frameInfo.overrideMetallic;
		material.roughness = frameInfo.overrideRoughness;
	}

	float ndh = max(0, dot(halfV, normal));
	float ndl = max(0, dot(frameInfo.lightDir, normal));
	float ndv = max(1e-4, dot(eye, normal));
	float hdv = max(0, dot(halfV, eye));

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
		ao,
		ndh, ndl, ndv, hdv);
	
	vec3 pxlColor = mainLight + ambientLight;
	if(material.emissiveTexture >= 0)
	{
		uint index = material.emissiveTexture;
		pxlColor += texture(textures[index], vPxlTexCoord).xyz;
	}

    outColor = vec4(pxlColor, 1.0);
}