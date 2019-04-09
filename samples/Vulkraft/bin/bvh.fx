// BVH raytracing
#ifndef _BVH_FX_
#define _BVH_FX_

#include "ImplicitRay.fx"

struct BVHNode
{
	int childOffset;
	int nextOffset;
	int leafMask; // Higher (30) bits are the index of triengles
	Box AABB1;
	Box AABB2;
};

struct Triangle
{
	ivec3 indices;
};

const int NUM_TRIS = 8;
Triangle triangles[NUM_TRIS] = 
{
	{ ivec3(0,1,2) },
	{ ivec3(0,2,3) },
	{ ivec3(0,3,4) },
	{ ivec3(0,4,1) },
	{ ivec3(2,1,5) },
	{ ivec3(3,2,5) },
	{ ivec3(4,3,5) },
	{ ivec3(1,4,5) }
};

vec3 vertices[6] = 
{
	vec3(0.0, 0.0, 0.0),
	vec3(0.5, 0.5, 0.0),
	vec3(0.0, 0.5, 0.5),
	vec3(-0.5, 0.5, 0.0),
	vec3(0.0, 0.5,-0.5),
	vec3(0.0, 1.0, 0.0)
};

BVHNode bvhTree[7] =
{
	// level 0
	{
		1,
		1,
		0,
		{vec3(-0.5,0.0,-0.5), vec3(0.5,0.5,0.5)},
		{vec3(-0.5,0.5,-0.5), vec3(0.5,1.0,0.5)}
	},
	// Level 1
	{
		2,
		1,
		0,
		{vec3(-0.5,0.0,-0.5), vec3(0.0,0.5,0.5)},
		{vec3( 0.0,0.0,-0.5), vec3(0.5,0.5,0.5)}
	},
	{
		3,
		0,
		0,
		{vec3(-0.5,0.5,-0.5), vec3(0.0,1.0,0.5)},
		{vec3( 0.0,0.5,-0.5), vec3(0.5,1.0,0.5)}
	},
	// level 2
	{
		0,
		1,
		3,
		{vec3(-0.5,0.0,-0.5), vec3(0.0,0.5,0.0)},
		{vec3(-0.5,0.0, 0.0), vec3(0.0,0.5,0.5)}
	},
	{
		0,
		-2,
		3 | 2<<2,
		{vec3( 0.0,0.0,-0.5), vec3(0.5,0.5,0.0)},
		{vec3( 0.0,0.0, 0.0), vec3(0.5,0.5,0.5)}
	},
	{
		0,
		1,
		3 | 4<<2,
		{vec3(-0.5,0.5,-0.5), vec3(0.0,1.0,0.0)},
		{vec3(-0.5,0.5, 0.0), vec3(0.0,1.0,0.5)}
	},
	{
		0,
		0,
		3 | 6<<2,
		{vec3( 0.0,0.5,-0.5), vec3(0.5,1.0,0.0)},
		{vec3( 0.0,0.5, 0.0), vec3(0.5,1.0,0.5)}
	}
};

float hitTriangle(Triangle tri, vec3 ro, vec3 rd, out vec3 normal, float tMax)
{
	vec3 v0 = vertices[tri.indices.x];
	vec3 v1 = vertices[tri.indices.y];
	vec3 v2 = vertices[tri.indices.z];

	vec3 h0 = v0 - ro;
	vec3 h1 = v1 - ro;
	vec3 h2 = v2 - ro;

	vec3 a0 = cross(h0,h1);
	vec3 a1 = cross(h1,h2);
	vec3 a2 = cross(h2,h0);

	vec3 e1 = v2-v1;
	vec3 e0 = v1-v0;
	normal = cross(e0,e1);

	if((dot(a0,rd) <= 0) && (dot(a1,rd) <= 0) && (dot(a2,rd) <= 0))
	{
		float t = dot(normal, h0) / dot(rd, normal);
		if(t >= 0 && t <= tMax)
		{
			normal = normalize(normal);
			return t;
		}
	}

	return -1.0;
}


float hitBVH(vec3 ro, vec3 rd, out vec3 normal, float tMax)
{
	ImplicitRay ir;
	toImplicit(ro, rd, ir);
	float t = -1.0;

	int curNodeNdx = 0;
	BVHNode curNode;

	for(;;)
	{
		curNode = bvhTree[curNodeNdx];
		// Child A
		// Is leaf?
		vec3 tNormal;
		int triNdx = curNode.leafMask>>2;
		if((curNode.leafMask & 1) != 0)
		{
			float tTri = hitTriangle(triangles[triNdx], ro, rd, tNormal, tMax);
			triNdx+=1;
			if(tTri >= 0)
			{
				tMax = tTri;
				t = tTri;
				normal = tNormal;
			}
		}
		else
		{
			if(hitBox(curNode.AABB1, ir, tNormal, tMax) >= 0)
			{
				curNodeNdx += curNode.childOffset;
				continue;
			}
		}

		if((curNode.leafMask & 2) != 0)
		{
			float tTri = hitTriangle(triangles[triNdx], ro, rd, tNormal, tMax);
			if(tTri >= 0)
			{
				tMax = tTri;
				t = tTri;
				normal = tNormal;
			}
		}
		else
		{
			if(hitBox(curNode.AABB2, ir, tNormal, tMax) >= 0)
			{
				curNodeNdx += curNode.childOffset;
				continue;
			}
		}

		if(curNode.nextOffset == 0)
			break;
		
		curNodeNdx = curNodeNdx + curNode.nextOffset;
	}

	return t;
}


#endif // _BVH_FX