#ifndef _RT_COMMON_FX_
#define _RT_COMMON_FX_

// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Input uniforms
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform mat4 uCamWorld;
layout(location = 3) uniform mat4 uViewMtx;
layout(location = 4) uniform mat4 uProj;
layout(location = 5) uniform sampler2D uTexturePack;
layout(location = 6) uniform vec4 uNoiseOffset;
layout(location = 9) uniform vec4 uFovTg;

const float HalfPi = 1.57079637050628662109375f;
const float TwoPi = 6.2831852436065673828125f;

#include "AABB.fx"
#include "bvh.fx"

vec3 worldSpaceRay(mat4 camWorld, vec2 clipSpacePos)
{
	vec4 viewSpaceRay = vec4(clipSpacePos*uFovTg.xy, -1.0, 0.0);
	return (camWorld * viewSpaceRay).xyz;
}

void worldRayFromPixel(mat4 camWorld, ivec2 pixel_coord, out vec3 ro, out vec3 rd)
{
	vec2 uvs = (vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y) + vec2(0.5)) / uWindow.xy;
	ro = (camWorld * vec4(0,0,0,1.0)).xyz;
	rd = worldSpaceRay(camWorld, 2*uvs-1);
}

vec3 worldPosFromSS(mat4 view, vec2 ssPos, float t)
{
	vec3 vsPos = vec3(ssPos*uFovTg.xy, -1.0)*t;
	mat4 camWorld = inverse(view);
	vec4 wsPos = camWorld * vec4(vsPos,1);
	return wsPos.xyz;
}

vec3 worldPosFromUV(mat4 view, vec2 uvs, float t)
{
	return worldPosFromSS(view, uvs * 2 - 1, t);
}

vec3 worldPosFromPixel(mat4 view, ivec2 pixel_coords, float t)
{
	vec2 uvs = vec2(pixel_coords.x+0.5, pixel_coords.y+0.5) / uWindow.xy;
	return worldPosFromUV(view, uvs, t);
}

#ifdef GBUFFER
vec3 fetchAlbedo(vec3 worldPos, vec3 worldNormal, float t, int lodBias)
{
	return vec3(0.8);
	// Choose material
	/*ivec2 tileOffset = ivec2(0,0);
	//if(worldPos.y > 0.01)
	//	tileOffset = ivec2(2,4);
	//if(worldPos.y > 1.99)
	//	tileOffset = ivec2(4,0);
	// Compute uvs
	vec2 texUV;
	if(worldNormal.y > 0.5 || worldNormal.y < -0.5)
	{
		texUV = fract(worldPos.xz);
	}
	else
	{
		vec3 bitan = vec3(0.0,-1.0,0.0);
		vec3 tan = cross(bitan, worldNormal);
		float u = dot(worldPos.xyz,tan);
		float v = dot(worldPos.xyz,bitan);
		texUV = fract(vec2(u, v));
	}
	int texLOD = max(0,min(4,lodBias + int(log2(t/7))));
	int sampleScale = (16>>texLOD);
	texUV = (texUV+tileOffset)*sampleScale;

	return texelFetch(uTexturePack, ivec2(texUV.x,texUV.y), texLOD).xyz;*/
}
#endif

vec3 sunDir = normalize(vec3(-1.0,2.0,2.0));
float sunDiskSize = 0.0125;
vec3 sunLight = 2*vec3(1.0,1.0,0.8);
vec3 skyColor(vec3 dir)
{
	return mix(1*vec3(0.1, 0.4, 0.80), 2*vec3(0.3,0.7,1.0), max(0.0,dot(normalize(dir),sunDir)));
}

float hit(in vec3 ro, in vec3 rd, out vec3 normal, float tMax)
{
	float t = -1.0;

	// Hit ground plane
	if(rd.y < 0.0)
	{
		normal = vec3(0.0,1.0,0.0);
		t = -ro.y / rd.y;
		tMax = t;
	}

	// BVH
	{
		vec3 tNormal;
		float tModel = closestHitBVH(ro, rd, tNormal, tMax);
		if(tModel > 0)
		{
			t = tModel;
			tMax = tModel;
			normal = tNormal;
		}
	}

	return t;
}

float hit_any(in vec3 ro, in vec3 rd, float tMax)
{
	if(rd.y <= 0.0) // Hit plane
	{
		return 1.0;
	}

	// BVH
	{
		float tModel = hitBVH(ro, rd, tMax);
		if(tModel > 0)
		{
			return tModel;
		}
	}

	return -1.0;
}

// Pixar's method for orthonormal basis generation
void branchlessONB(in vec3 n, out vec3 b1, out vec3 b2)
{
	float sign = n.z > 0.0 ? 1.0 : -1.0;
	const float a = -1.0f / (sign + n.z);
	const float b = n.x * n.y * a;
	b1 = vec3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
	b2 = vec3(b, sign + n.y * n.y * a, -n.y);
}

vec3 orthonormalVec3(in vec3 normal)
{
	float fDot = abs(dot(normal, vec3(0, 1, 0)));
  	if (fDot < 0.999f)
    	return cross(normal, vec3(0, 1, 0));
  	return cross(normal, vec3(1, 0, 0));
}

void onb(in vec3 n, out vec3 b1, out vec3 b2)
{
	b1 = orthonormalVec3(n);
	b2 = cross(n,b1);
}

vec3 randomUnitVector(in vec2 seed)
{
	float theta = TwoPi*seed.x;
	float z = 2*seed.y-1;
	float horRad = sqrt(1-z*z);
	return vec3(
		cos(theta)*horRad,
		z,
		sin(theta)*horRad
		);
}

vec3 lambertianDirection(in vec3 normal, in vec2 seed)
{
	return normal + 0.985 * randomUnitVector(seed);
}

#endif // _RT_COMMON_FX_