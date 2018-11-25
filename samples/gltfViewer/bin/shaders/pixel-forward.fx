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
layout(location = 29) uniform sampler2D uDepthMap;
layout(location = 0) uniform mat4 uWorldViewProjection;
layout(location = 1) uniform mat4 uWorld;
in vec3 vtxWorldPos;

// Global state
layout(location = 3) uniform float uEV;

const float PI = 3.1415927410125732421875;
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
	float ao;
};

vec4 shadeSurface(ShadeInput inputs);
vec3 getSampledNormal(vec3 tangent, vec3 bitangent, vec3 normal);

//------------------------------------------------------------------------------	
void branchlessONB(vec3 n, out vec3 b1, out vec3 b2)
{
	float sign = (n.z>=0)?1.0:-1.0;
	float a = -1.0f / (sign + n.z);
	float b = n.x * n.y * a;
	b1 = vec3(1.0 + sign * n.x * n.x * a, sign * b, -sign * n.x);
	b2 = vec3(b, sign + n.y * n.y * a, -n.y);
}

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
	branchlessONB(shadeInput.normal, shadeInput.tangent, shadeInput.bitangent);
#endif
}

//------------------------------------------------------------------------------	
void main (void) {
	// Build shader inputs
	ShadeInput shadingInputs;
	vec2 uv = gl_FragCoord.xy/textureSize(uDepthMap, 0);

	computeTangentSpace(shadingInputs);

#ifdef sampler2D_uShadowMap
	shadingInputs.shadowPos = vtxShadowPos.xyz;
#endif
	shadingInputs.eye = normalize(vtxWsEyeDir);
	float ndv = dot(shadingInputs.eye,shadingInputs.normal);
	if(ndv < 0.0)
		shadingInputs.eye = reflect(shadingInputs.eye, shadingInputs.normal);
	shadingInputs.ndv = max(1e-8,dot(shadingInputs.eye, shadingInputs.normal));

	// Ambient occlusion
	mat4 viewProj = uWorldViewProjection * inverse(uWorld);
	vec4 rootPos = uWorld * vec4(vtxWorldPos, 1.0);
	vec3 samplePos = rootPos.xyz;
	//vec4 ssSamplePos = viewProj * vec4(samplePos, 1.0);

	vec3 offsets[9];
	offsets[0] = shadingInputs.normal;
	offsets[1] = 0.5*normalize(0.9*shadingInputs.normal+0.5*shadingInputs.tangent);
	offsets[2] = normalize(0.9*shadingInputs.normal+0.5*shadingInputs.bitangent);
	offsets[3] = 0.3*normalize(0.9*shadingInputs.normal-0.5*shadingInputs.tangent);
	offsets[4] = normalize(0.9*shadingInputs.normal-0.5*shadingInputs.bitangent);
	offsets[5] = 0.7*normalize(0.9*shadingInputs.normal+1.0*shadingInputs.tangent);
	offsets[6] = 0.1*normalize(0.9*shadingInputs.normal+1.0*shadingInputs.bitangent);
	offsets[7] = normalize(0.9*shadingInputs.normal-1.0*shadingInputs.tangent);
	offsets[8] = 0.4*normalize(0.9*shadingInputs.normal-1.0*shadingInputs.bitangent);
	float occluding = 0.0;
	float aoRadius = 0.005;
	for(int i = 0; i < 1; ++i)
	{
		vec3 wsSamplePos = vtxWorldPos + aoRadius*offsets[i];
		vec4 ssSamplePos = viewProj * vec4(wsSamplePos, 1.0);
		vec3 ssSample = ssSamplePos.xyz;
		float rasterDepth = texture(uDepthMap, ssSample.xy/ssSamplePos.w*0.5+0.5).r * 2.0 - 1.0;
		vec4 rasterPoint = vec4(ssSample.xy/ssSamplePos.w, rasterDepth, 1.0)*ssSamplePos.w;
		vec4 wsRasterPoint = inverse(viewProj) * rasterPoint;
		vec3 wsDistance = wsRasterPoint.xyz - vtxWorldPos;
		float rangeCheck = smoothstep(0.0,1.0, dot(wsDistance,wsDistance) / (aoRadius*aoRadius));
		
		occluding += dot(wsDistance, shadingInputs.normal) > 0.0001 ? 1.0*rangeCheck : 0.0;
	}
	float ao = 1.0-(occluding/9);
	shadingInputs.ao = ao;
	
	// ---- Shading ----
	// Compute actual lighting
	vec4 pbrColor = uEV * shadeSurface(shadingInputs);
	//if(pbrColor.b > 1.0)
	//	pbrColor.rgb = vec3(1.0, 0.0, 0.0);
	//pbrColor.xyz = pbrColor.xyz / (1.0+pbrColor.xyz);
	vec3 toneMapped = pow(pbrColor.xyz, vec3(0.4545));

	outColor = vec4(toneMapped, pbrColor.a);
	//outColor = vec4(shadingInputs.normal, pbrColor.a);
	//outColor = vec4(vec3(shadingInputs.ao), pbrColor.a);
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif // PXL_SHADER
