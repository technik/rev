// BVH raytracing
#ifndef _BVH_FX_
#define _BVH_FX_

#include "ImplicitRay.fx"

struct RelativeBBox
{
	ivec4 min;
	ivec4 max;
};

struct BVHNode
{
	int childOffset;
	int nextOffset;
	int leafMask; // Higher (30) bits are the index of triengles
	Box AABB[2];
};

layout(std430, binding = 7) buffer NdxBuffer
{
	int indices[];
} modelFaces;

layout(std430, binding = 8) buffer VtxBuffer
{
	float pos[];
} modelVertices;

vec3 octaBBoxMin = vec3(-0.5,0.0,-0.5);
vec3 octaBBoxSize = vec3(0.5);

BVHNode bvhTree[7] =
{
	// level 0
	{
		1,
		1,
		0,
		{{vec3(0), vec3(2,1,2)},
		{vec3(0,1,0), vec3(2)}}
	},
	// Level 1
	{
		2,
		1,
		0,
		{{vec3(0), vec3(1,1,2)},
		{vec3( 1,0.0,0), vec3(2,1,2)}}
	},
	{
		3,
		0,
		0,
		{{vec3(0,1,0), vec3(1,2,2)},
		{vec3( 1,1,0), vec3(2)}}
	},
	// level 2
	{
		0,
		1,
		3,
		{{vec3(0), vec3(1)},
		{vec3(0,0.0, 1), vec3(1,1,2)}}
	},
	{
		0,
		-2,
		3 | 2<<2,
		{{vec3( 1,0.0,0), vec3(2,1,1)},
		{vec3( 1,0.0, 1), vec3(2,1,2)}}
	},
	{
		0,
		1,
		3 | 4<<2,
		{{vec3(0,1,0), vec3(1,2,1)},
		{vec3(0,1,1), vec3(1,2,2)}}
	},
	{
		0,
		0,
		3 | 6<<2,
		{{vec3( 1,1,0), vec3(2,2,1)},
		{vec3( 1), vec3(2)}}
	}
};

float hitTriangle(int triNdx, vec3 ro, vec3 rd, float tMax)
{
	int v0Ndx = 3*modelFaces.indices[triNdx*3+0];
	int v1Ndx = 3*modelFaces.indices[triNdx*3+1];
	int v2Ndx = 3*modelFaces.indices[triNdx*3+2];
	vec3 v0 = vec3(modelVertices.pos[v0Ndx], modelVertices.pos[v0Ndx+1], modelVertices.pos[v0Ndx+2]);
	vec3 v1 = vec3(modelVertices.pos[v1Ndx], modelVertices.pos[v1Ndx+1], modelVertices.pos[v1Ndx+2]);
	vec3 v2 = vec3(modelVertices.pos[v2Ndx], modelVertices.pos[v2Ndx+1], modelVertices.pos[v2Ndx+2]);

	vec3 e1 = v2-v1;
	vec3 e0 = v1-v0;
	vec3 normal = cross(e0,e1);
	if(dot(normal, rd) > 0)
		return -1.0;

	vec3 h0 = v0 - ro;
	vec3 h1 = v1 - ro;
	vec3 h2 = v2 - ro;

	vec3 a0 = cross(h0,h1);
	vec3 a1 = cross(h1,h2);
	vec3 a2 = cross(h2,h0);

	if((dot(a0,rd) <= 0) && (dot(a1,rd) <= 0) && (dot(a2,rd) <= 0))
	{
		float t = dot(normal, h0) / dot(rd, normal);
		if(t >= 0 && t <= tMax)
		{
			return t;
		}
	}

	return -1.0;
}

float closestHitTriangle(int triNdx, vec3 ro, vec3 rd, out vec3 normal, float tMax)
{
	int v0Ndx = 3*modelFaces.indices[triNdx*3+0];
	int v1Ndx = 3*modelFaces.indices[triNdx*3+1];
	int v2Ndx = 3*modelFaces.indices[triNdx*3+2];
	vec3 v0 = vec3(modelVertices.pos[v0Ndx], modelVertices.pos[v0Ndx+1], modelVertices.pos[v0Ndx+2]);
	vec3 v1 = vec3(modelVertices.pos[v1Ndx], modelVertices.pos[v1Ndx+1], modelVertices.pos[v1Ndx+2]);
	vec3 v2 = vec3(modelVertices.pos[v2Ndx], modelVertices.pos[v2Ndx+1], modelVertices.pos[v2Ndx+2]);

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

Box getChildBBox(BVHNode node, int ndx)
{
	Box localBBox = node.AABB[ndx];
	Box scaledBBox;
	scaledBBox.min = localBBox.min.xyz * octaBBoxSize;
	scaledBBox.max = localBBox.max.xyz * octaBBoxSize;
	return scaledBBox;
}

float hitBVH(vec3 ro, vec3 rd, float tMax)
{
	ImplicitRay ir;
	toImplicit(ro- octaBBoxMin, rd, ir);
	float t = -1.0;

	int curNodeNdx = 0;
	BVHNode curNode;

	float tBox;
	for(;;)
	{
		curNode = bvhTree[curNodeNdx];
		// Child A
		// Is leaf?
		int triNdx = curNode.leafMask>>2;
		if((curNode.leafMask & 1) != 0)
		{
			float tTri = hitTriangle(triNdx, ro, rd, tMax);
			if(tTri >= 0)
			{
				return 1.0;
			}
			triNdx+=1;
		}
		else
		{
			tBox = hitBoxAny(getChildBBox(curNode,0), ir, tMax);
			if(tBox >= 0)
			{
				curNodeNdx += curNode.childOffset;
				continue;
			}
		}

		if((curNode.leafMask & 2) != 0)
		{
			float tTri = hitTriangle(triNdx, ro, rd, tMax);
			if(tTri >= 0)
			{
				return 1.0;
			}
		}
		else
		{
			tBox = hitBoxAny(getChildBBox(curNode,1), ir, tMax);
			if(tBox >= 0)
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

float closestHitBVH(vec3 ro, vec3 rd, out vec3 normal, float tMax)
{
	ImplicitRay ir;
	toImplicit(ro- octaBBoxMin, rd, ir);
	float t = -1.0;

	int curNodeNdx = 0;
	BVHNode curNode;

	float tBox;
	for(;;)
	{
		curNode = bvhTree[curNodeNdx];
		// Child A
		// Is leaf?
		vec3 tNormal;
		int triNdx = curNode.leafMask>>2;
		if((curNode.leafMask & 1) != 0)
		{
			float tTri = closestHitTriangle(triNdx, ro, rd, tNormal, tMax);
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
			tBox = hitBox(getChildBBox(curNode,0), ir, tNormal, tMax);
			if(tBox >= 0)
			{
				curNodeNdx += curNode.childOffset;
				continue;
			}
		}

		if((curNode.leafMask & 2) != 0)
		{
			float tTri = closestHitTriangle(triNdx, ro, rd, tNormal, tMax);
			if(tTri >= 0)
			{
				tMax = tTri;
				t = tTri;
				normal = tNormal;
			}
		}
		else
		{
			tBox = hitBox(getChildBBox(curNode,1), ir, tNormal, tMax);
			if(tBox >= 0)
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