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

//------------------------------------------------------------------------------
float GGX_NDF(float ndh, float a)
{
    float a2     = a*a;
    float ndh2 = ndh*ndh;
	
    float denom  = ndh2 * (a2 - 1.0) + 1.0;
    denom        = PI * denom * denom;
	
    return a2 / denom;
}

//------------------------------------------------------------------------------
// Actually: GGX divided by ndv
// SIMD optimized version
vec2 GeometrySchlickGGX(vec2 ndv_ndl, float roughness)
{
	float k = roughness/2.0;
    vec2 denom = ndv_ndl*(1.0-k) + k;
	
    return 1.0 / denom;
}

//------------------------------------------------------------------------------
// Actually: Smith GGX divided by (ndv * ndl)
// SIMD optimized version
float GeometrySmithGGX(vec2 ndv_ndl, float roughness)
{
    vec2 ggx  = GeometrySchlickGGX(ndv_ndl, roughness);
    return ggx.x * ggx.y;
}

#define SPHERICAL_GAUSIAN_SCHLICK
//------------------------------------------------------------------------------
float fresnelSchlick(float ndv)
{
#ifdef SPHERICAL_GAUSIAN_SCHLICK
	// Schlick with Spherical Gaussian approximation
	// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
	return pow(2.0, (-5.55473*ndv - 6.98316) * ndv);
#else // !SPHERICAL_GAUSIAN_SCHLICK
	//return pow(ndl, 5.0);
	float n = 1.0-ndv;
	float n2 = n*n;
	return n2*n2*n;
#endif // !SPHERICAL_GAUSIAN_SCHLICK
}

float normal_distrib(
  float ndh,
  float Roughness)
{
  // use GGX / Trowbridge-Reitz, same as Disney and Unreal 4
  // cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
  float alpha = Roughness * Roughness;
  float tmp = alpha / max(1e-8,(ndh*ndh*(alpha*alpha-1.0)+1.0));
  return tmp * tmp * INV_PI;
}

vec3 fresnel(
  float vdh,
  vec3 F0)
{
  // Schlick with Spherical Gaussian approximation
  // cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
  float sphg = pow(2.0, (-5.55473*vdh - 6.98316) * vdh);
  return F0 + (vec3(1.0) - F0) * sphg;
}

float G1(
  float ndw, // w is either Ln or Vn
  float k)
{
  // One generic factor of the geometry function divided by ndw
  // NB : We should have k > 0
  return 1.0 / ( ndw*(1.0-k) +  k );
}

float visibility(
  float ndl,
  float ndv,
  float Roughness)
{
  // Schlick with Smith-like choice of k
  // cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
  // visibility is a Cook-Torrance geometry function divided by (n.l)*(n.v)
  float k = max(Roughness * Roughness * 0.5, 1e-5);
  return G1(ndl,k)*G1(ndv,k);
}

vec3 cook_torrance_contrib(
  float vdh,
  float ndh,
  float ndl,
  float ndv,
  vec3 Ks,
  float Roughness)
{
  // This is the contribution when using importance sampling with the GGX based
  // sample distribution. This means ct_contrib = ct_brdf / ggx_probability
  return fresnel(vdh,Ks) * (visibility(ndl,ndv,Roughness) * vdh * ndl / ndh );
  //return vec3(visibility(ndl,ndv,Roughness) * vdh * ndl / ndh );
}

vec3 importanceSampleGGX(vec2 Xi, vec3 T, vec3 B, vec3 N, float roughness)
{
  float a = roughness*roughness;
  float cosT = sqrt((1.0-Xi.y)/(1.0+(a*a-1.0)*Xi.y));
  float sinT = sqrt(1.0-cosT*cosT);
  float phi = Xi.x; // premultiplied by 2*pi
  return
    T * (sinT*cos(phi)) +
    B * (sinT*sin(phi)) +
    N *  cosT;
}

float probabilityGGX(float ndh, float vdh, float Roughness)
{
  return normal_distrib(ndh, Roughness) * ndh / (4.0*vdh);
}

#endif // PBR_FX