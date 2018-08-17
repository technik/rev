//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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
#ifdef PXL_SHADER

out lowp vec4 outColor;
#ifdef VTX_TANGENT_SPACE
in vec4 vtxTangent;
#endif
in vec3 vtxNormal;
in vec3 vtxWsEyeDir;
in vec2 vTexCoord;
#ifdef sampler2D_uShadowMap
in vec3 vtxShadowPos;
#endif

// Global state
layout(location = 3) uniform float uEV;

const float PI = 3.14159265359;
const float TWO_PI = 6.283185307179586;
const float HALF_PI = 1.5707963267948966;
const float INV_PI = 0.3183098861837907;

struct ShadeInput
{
	float ndv; // Normal dot view
	// World space vectors
	vec3 tangent;
	vec3 bitangent;
	vec3 normal;
	vec3 shadowPos;
	vec3 eye;
};

vec4 shadeSurface(ShadeInput inputs);
vec3 getSampledNormal(vec3 tangent, vec3 bitangent, vec3 normal);

//------------------------------------------------------------------------------
void computeTangentSpace(inout ShadeInput shadeInput)
{
	// Tangent space
#ifdef VTX_TANGENT_SPACE
	vec3 tangent = normalize(vtxTangent.xyz);
	vec3 normal = normalize(vtxNormal);
	vec3 bitangent = cross(normal, tangent) * vtxTangent.w;
	normal = getSampledNormal(tangent, bitangent, normal);

	// Recompute tangent space around sampled normal
	tangent = normalize(
		tangent
		-dot(normal,tangent)*normal
		);
	bitangent = normalize(
		bitangent
		-dot(normal,bitangent)*normal
		-dot(tangent,bitangent)*tangent
		);

	shadeInput.tangent = tangent;
	shadeInput.bitangent = bitangent;
	shadeInput.normal = normal;
#else
	shadeInput.normal = normalize(vtxNormal);
#endif
}

//------------------------------------------------------------------------------	
void main (void) {
	// Build shader inputs
	ShadeInput shadingInputs;

	computeTangentSpace(shadingInputs);

#ifdef sampler2D_uShadowMap
	shadingInputs.shadowPos = vtxShadowPos.xyz;
#endif
	shadingInputs.eye = normalize(vtxWsEyeDir);
	float ndv = dot(shadingInputs.eye,shadingInputs.normal);
	if(ndv < 0.0)
		shadingInputs.eye = reflect(shadingInputs.eye, shadingInputs.normal);
	shadingInputs.ndv = max(1e-8,dot(shadingInputs.eye, shadingInputs.normal));

	// ---- Shading ----
	// Compute actual lighting
	vec4 pbrColor = uEV * shadeSurface(shadingInputs);

	//vec3 toneMapped = pbrColor.xyz / (pbrColor.xyz + 1);
	
	//outColor = vec4(toneMapped, pbrColor.a);
	outColor = vec4(pbrColor.xyz, pbrColor.a);
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif // PXL_SHADER
