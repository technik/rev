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

#if defined(sampler2D_uNormalMap) && defined(VTX_UV_FLOAT) && defined(VTX_TANGENT_FLOAT)
#define VTX_TANGENT_SPACE
#endif

#ifdef VTX_SHADER

//------------------------------------------------------------------------------
// Vertex attributes
//------------------------------------------------------------------------------
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 msNormal;
#ifdef VTX_TANGENT_FLOAT
layout(location = 2) in vec4 msTangent;
#endif
#ifdef VTX_UV_FLOAT
layout(location = 3) in vec2 texCoord;
#endif

layout(location = 0) uniform mat4 uWorldViewProjection;
layout(location = 1) uniform mat4 uWorld;

//------------------------------------------------------------------------------
// Per vertex output
//------------------------------------------------------------------------------
out vec3 vtxWsNormal;
#ifdef VTX_UV_FLOAT
out vec2 vTexCoord;
#endif
#ifdef VTX_TANGENT_SPACE
out vec4 vtxTangent;
#endif

//------------------------------------------------------------------------------
void main ( void )
{
	// Textures
#ifdef VTX_UV_FLOAT
	vTexCoord = texCoord;
#endif

	// Tangent space
	mat3 worldRot = mat3(uWorld);
	mat3 invTransWorldRot = inverse(transpose(worldRot));
	vtxWsNormal = invTransWorldRot * msNormal;
#ifdef VTX_TANGENT_SPACE
	vtxTangent = vec4(invTransWorldRot * msTangent.xyz, msTangent.w);
#endif
	// Position
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif // VTX_SHADER

//----------------------------------------------------------------------------
// Fragment shader
//----------------------------------------------------------------------------

#ifdef PXL_SHADER

#include "pbr.fx"

// Pixel outputs
layout(location = 0) out lowp vec4 outWsNormal;
layout(location = 1) out lowp vec4 outAlbedo;
layout(location = 2) out lowp vec4 outSpecular;

//------------------------------------------------------------------------------
vec3 computeWsNormal()
{
	// Tangent space
#ifdef VTX_TANGENT_SPACE
	vec3 tangent = normalize(vtxTangent.xyz);
	vec3 normal = normalize(vtxWsNormal);
	vec3 bitangent = normalize(cross(normal, tangent) * vtxTangent.w);
	return getSampledNormal(tangent, bitangent, normal);
#else
	return normalize(vtxWsNormal);
#endif
}

//------------------------------------------------------------------------------	
void main (void) {
	outWsNormal = vec4(computeWsNormal(), 0.0);

	PBRParams pbr = getPBRParams();
	outSpecular = pbr.specular_r;

	outAlbedo = pbr.albedo;
}

#endif // PXL_SHADER