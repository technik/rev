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
#ifndef _PUSH_CONSTANTS_GLSL_
#define _PUSH_CONSTANTS_GLSL_

layout(push_constant,scalar) uniform Constants
{
    mat4 proj;
    mat4 view;
	vec3 lightDir;
	vec3 ambientColor;
	vec3 lightColor;

	uint renderFlags;
	// Material override parameters
	vec3 overrideBaseColor;
	float overrideMetallic;
	float overrideRoughness;
	float overrideClearCoat;
} frameInfo;

#define RF_OVERRIDE_MATERIAL (1<<0)
#define RF_ENV_PROBE = (1<<1)
#define RF_DISABLE_AO (1 << 2)
#define RF_NO_NORMAL_MAP (1<<3)

bool renderFlag(uint flag)
{
	return (frameInfo.renderFlags & flag) > 0;
}

#endif // _PUSH_CONSTANTS_GLSL_