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
#ifndef PBR_FX
#define PBR_FX
// Common pbr code

#ifdef vec4_uBaseColor
layout(location = 14) uniform vec4 uBaseColor;
#endif
#ifdef sampler2D_uBaseColorMap
layout(location = 11) uniform sampler2D uBaseColorMap;
#endif

// Material
#ifdef sampler2D_uEnvironment
layout(location = 7) uniform sampler2D uEnvironment;
layout(location = 8) uniform sampler2D uEnvBRDF;
layout(location = 18) uniform float numEnvLevels;
#endif

#ifdef sampler2D_uFms
layout(location = 17) uniform sampler2D uFms;
#endif

#ifdef VTX_TANGENT_SPACE
//------------------------------------------------------------------------------
vec3 getSampledNormal(vec3 tangent, vec3 bitangent, vec3 normal)
{
  vec3 texNormal = (255.f/128.f)*texture(uNormalMap, vTexCoord).xyz - 1.0;
  
  //return normal;
  return normalize(
    tangent*texNormal.x +
    bitangent*texNormal.y +
    normal*max(texNormal.z, 1e-8)
  );
}
#endif


//---------------------------------------------------------------------------------------
const vec2 invAtan = vec2(0.159154943, -0.31830988);
vec2 sampleSpherical(vec3 v)
{
  vec2 uv = vec2(atan(-v.x, v.z), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//---------------------------------------------------------------------------------------
struct LocalVectors
{
  vec3 eye;
  vec3 tangent;
  vec3 bitangent;
  vec3 normal;
};

//---------------------------------------------------------------------------------------
vec3 getIrradiance(vec3 dir)
{
#if defined(sampler2D_uIrradiance) && !defined(Furnace)
  return textureLod(uEnvironment, sampleSpherical(dir), numEnvLevels).xyz;
#else
  return vec3(1.0);
#endif
}

//---------------------------------------------------------------------------------------
vec3 getRadiance(vec3 dir, float lodLevel)
{
#if defined(sampler2D_uIrradiance) && !defined(Furnace)
  return textureLod(uEnvironment, sampleSpherical(dir), lodLevel).xyz;
#else
  return vec3(1.0);
#endif
}

//---------------------------------------------------------------------------------------
vec4 getBaseColor()
{
#if defined(sampler2D_uBaseColorMap) && defined(vec4_uBaseColor)
  vec4 baseColorTex = texture(uBaseColorMap, vTexCoord);
  vec4 baseColor = baseColorTex*uBaseColor;
#else
  #if defined(sampler2D_uBaseColorMap)
    vec4 baseColor = texture(uBaseColorMap, vTexCoord);
    baseColor = vec4(pow(baseColor.r, 2.2), pow(baseColor.g, 2.2), pow(baseColor.b, 2.2), baseColor.a);
  #else
    #if defined(vec4_uBaseColor)
      vec4 baseColor = uBaseColor;
    #else
      vec4 baseColor = vec4(1.0);
    #endif
  #endif
#endif
  return baseColor;
}

#endif // PBR_FX