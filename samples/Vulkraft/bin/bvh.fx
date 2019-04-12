// BVH raytracing
#ifndef _BVH_FX_
#define _BVH_FX_

#include "ImplicitRay.fx"


struct BVHNode
{
	int nextOffset;
	int leafMask; // Higher (30) bits are the index of triengles
	ivec3 AABB[2];
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

Box getChildBBox(BVHNode node, int ndx)
{
	ivec3 localBBox = node.AABB[ndx];
	ivec3 relMin = localBBox & 0xffff;
	ivec3 relMax = localBBox >> 16;
	Box scaledBBox;
	scaledBBox.min = relMin * octaBBoxSize;
	scaledBBox.max = relMax * octaBBoxSize;
	return scaledBBox;
}

int getChildOffset(BVHNode node)
{
	return node.nextOffset>>16;
}

int getNextOffset(BVHNode node)
{
	int lowWord = node.nextOffset & 0xffff;
	return (lowWord & (1<<15)) == 0? lowWord : (lowWord | 0xffff0000);
}

BVHNode bvhTree[7] =
{
	// level 0
	{
		1<<16 | 1,
		0,
		{ivec3(0) + ivec3(2,1,2) << 16,
		ivec3(0,1,0) + ivec3(2) << 16}
	},
	// Level 1
	{
		2<<16 | 1,
		0,
		{ivec3(0) + ivec3(1,1,2) << 16,
		ivec3( 1,0.0,0)+ ivec3(2,1,2) << 16}
	},
	{
		3<<16 | 0,
		0,
		{ivec3(0,1,0)+ ivec3(1,2,2) << 16,
		ivec3( 1,1,0)+ ivec3(2) << 16}
	},
	// level 2
	{
		0 | 1,
		3,
		{ivec3(0)+ ivec3(1),
		ivec3(0,0.0, 1)+ ivec3(1,1,2)}
	},
	{
		0 | (-2&0xffff),
		3 | 2<<2,
		{ivec3( 1,0.0,0)+ ivec3(2,1,1),
		ivec3( 1,0.0, 1)+ ivec3(2,1,2)}
	},
	{
		0 | 1,
		3 | 4<<2,
		{ivec3(0,1,0)+ ivec3(1,2,1),
		ivec3(0,1,1)+ ivec3(1,2,2)}
	},
	{
		0 | 0,
		3 | 6<<2,
		{ivec3( 1,1,0)+ ivec3(2,2,1),
		ivec3( 1)+ ivec3(2)}
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
				curNodeNdx += getChildOffset(curNode);
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
				curNodeNdx += getChildOffset(curNode);
				continue;
			}
		}

		if(getNextOffset(curNode) == 0)
			break;
		
		curNodeNdx = curNodeNdx + getNextOffset(curNode);
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
				curNodeNdx += getChildOffset(curNode);
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
				curNodeNdx += getChildOffset(curNode);
				continue;
			}
		}

		if(getNextOffset(curNode) == 0)
			break;
		
		curNodeNdx = curNodeNdx + getNextOffset(curNode);
	}

	return t;
}
#endif // _BVH_FX