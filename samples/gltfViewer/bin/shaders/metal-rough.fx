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

// Metallic-rough pbr shader
//#define Furnace
#define BSDF

#ifdef PXL_SHADER

/*#ifdef VTX_TANGENT_SPACE
layout(location = 10) uniform sampler2D uNormalMap;
#endif
#ifdef sampler2D_uShadowMap
layout(location = 9) uniform sampler2D uShadowMap;
#endif
// Material
#ifdef sampler2D_uEnvironment
layout(location = 7) uniform sampler2D uEnvironment;
layout(location = 8) uniform sampler2D uEnvBRDF;
layout(location = 18) uniform float numEnvLevels;
#endif

#include "pbr.fx"
#include "ibl.fx"

// Lighting 
layout(location = 5) uniform vec3 uLightColor;
layout(location = 6) uniform vec3 uLightDir; // Direction toward light

//---------------------------------------------------------------------------------------
vec4 shadeSurface(ShadeInput inputs)
{

#ifdef sampler2D_uShadowMap
	vec3 surfacePos = inputs.shadowPos*0.5+0.5;
	float casterDepth = texture(uShadowMap, surfacePos.xy).x;
	float shadowHardness = 1.0;
	float shadowEffect = 1.0-shadowHardness*max(0.0, dot(-uLightDir, inputs.normal));
	float shadowMask = (casterDepth < min(1.0, surfacePos.z)) ? shadowEffect : 1.0;
	if((surfacePos.x < 0.0) || (surfacePos.x > 1.0)
		|| (surfacePos.y < 0.0) || (surfacePos.y > 1.0))
	{
		shadowMask = 1.0;
	}
#else
	float shadowMask = 1.0;
#endif

	float occlusion = inputs.ao;
#ifdef Furnace
	occlusion = 1.0;
	shadowMask = 1.0;
#endif
	if(baseColor.a < 0.5)
		discard;

	PBRParams pbr = getPBRParams();
#ifdef sampler2D_uEnvironment
	vec3 color = ibl(pbr.specular_r.xyz, inputs.normal, inputs.eye, pbr.albedo.xyz, pbr.specular_r.a, occlusion, shadowMask, inputs.ndv);
#else
	vec3 color = pbr.albedo.xyz;
#endif
	return vec4(color, baseColor.a);
}*/

#endif // PXL_SHADER