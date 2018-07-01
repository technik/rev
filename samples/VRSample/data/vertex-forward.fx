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
#ifdef VTX_POSITION_FLOAT
layout(location = VTX_POSITION_FLOAT) in vec3 vertex;
#endif
#ifdef VTX_NORMAL_FLOAT
layout(location = VTX_NORMAL_FLOAT) in vec3 msNormal;
#endif
#ifdef VTX_TANGENT_FLOAT
layout(location = VTX_TANGENT_FLOAT) in vec4 msTangent;
#endif
#ifdef VTX_UV_FLOAT
layout(location = VTX_UV_FLOAT) in vec2 texCoord;
#endif

layout(location = 0) uniform mat4 uWorldViewProjection;
layout(location = 1) uniform mat4 uWorld;
//layout(location = 2) uniform mat4 uMs2Shadow;
layout(location = 4) uniform vec3 uWsViewPos; // Direction toward viewpoint

#ifdef VTX_TANGENT_SPACE
out vec4 vtxTangent;
#endif

//------------------------------------------------------------------------------
// Per vertex output
//------------------------------------------------------------------------------
#ifdef VTX_NORMAL_FLOAT
out vec3 vtxNormal;
#endif
out vec3 vtxWsEyeDir;
#ifdef VTX_UV_FLOAT
out vec2 vTexCoord;
#endif
out vec3 vtxWsPos;

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
	vtxNormal = invTransWorldRot * msNormal;
#ifdef VTX_TANGENT_SPACE
	vtxTangent = vec4(invTransWorldRot * msTangent.xyz, msTangent.w);
#endif
	// Lighting vectors
	vtxWsPos = (uWorld * vec4(vertex,1.0)).xyz;
	vtxWsEyeDir = uWsViewPos - vtxWsPos;
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif // VTX_SHADER