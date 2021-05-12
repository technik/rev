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
#ifndef _PBR_GLSL_
#define _PBR_GLSL_

#define PI 3.1415927

float D_GGX(float NoH, float alpha) {
    float a2 = NoH * alpha;
    float k = alpha / max(1e-4, 1.0 - NoH * NoH + a2 * a2);
    return k * k * (1.0 / PI);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float alpha) {
    float a2 = alpha * alpha;
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
    return 0.5 / max(1e-4, GGXV + GGXL);
}

float V_SmithGGXCorrelatedFast(float NoV, float NoL, float alpha) {
    float GGXV = NoL * (NoV * (1.0 - alpha) + alpha);
    float GGXL = NoV * (NoL * (1.0 - alpha) + alpha);
    return 0.5 / max(1e-4, GGXV + GGXL);
}

vec3 F_Schlick(float u, vec3 f0) {
    float f = pow(1.0 - u, 5.0);
    return f + f0 * (1.0 - f);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

float pureMirrorBRDF(float ndh, float ndl, float ndv, float r)
{
	float D = D_GGX(ndh, r);
	float G = V_SmithGGXCorrelatedFast(ndv, ndl, r);
	return D*G;
}

vec3 specularBRDF(vec3 specularColor, float ndh, float ndl, float ndv, float hdv, float r)
{
	float s = pureMirrorBRDF(ndh, ndv, ndl, r);
	return F_Schlick(hdv, specularColor) * s;
}

#endif // _PBR_GLSL_