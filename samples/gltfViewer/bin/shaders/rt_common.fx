// Local work group

const float HalfPi = 1.57079637050628662109375f;
const float TwoPi = 6.2831852436065673828125f;

struct Box
{
	vec3 min;
	vec3 max;
};

struct ImplicitRay
{
	vec3 o;
	vec3 n;
	vec3 d;
};

struct Triangle
{
	ivec3 indices;
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

	vec3 e1 = normalize(v2-v1);
	vec3 e0 = normalize(v1-v0);
	normal = normalize(cross(e0,e1));

	if((dot(a0,rd) < 0) && (dot(a1,rd) < 0) && (dot(a2,rd) < 0))
	{
		float t = dot(normal, h0) / dot(rd, normal);
		if(t > 0 && t < tMax)
		{
			return t;
		}
	}

	return -1.0;
}

float hitBox(in Box b, in ImplicitRay r, out vec3 normal, float tMax)
{
	vec3 t1 = (b.min - r.o) * r.n;
	vec3 t2 = (b.max - r.o) * r.n;
	// Swapping the order of comparison is important because of NaN behavior
	vec3 tEnter = min(t2,t1); // Enters
	vec3 tExit = max(t1,t2); // Exits
	float maxEnter = max(tEnter.x,max(tEnter.y,max(tEnter.z,0.0))); // If nan, return second operand, which is never nan
	float minLeave = min(tExit.x, min(tExit.y, min(tExit.z, tMax))); // If nan, return second operand, which is never nan
	if(minLeave >= maxEnter)
	{
		if(maxEnter == tEnter.x)
		{
			normal = vec3(1.0, 0.0, 0.0);
		}
		else if(maxEnter == tEnter.y)
		{
			normal = vec3(0.0, 1.0, 0.0);
		}
		else
		{
			normal = vec3(0.0, 0.0, 1.0);
		}
		if(dot(normal,r.d) > 0)
			normal = -normal;
		return maxEnter;
	}
	else
	{
		return -1.0;
	}
}

struct BVHNode
{
	int childOffset;
	int nextOffset;
	int leafMask;
	Box AABB1;
	Box AABB2;
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
		2,
		-2,
		3,
		{vec3( 0.0,0.0,-0.5), vec3(0.5,0.5,0.0)},
		{vec3( 0.0,0.0, 0.0), vec3(0.5,0.5,0.5)}
	},
	{
		4,
		1,
		3,
		{vec3(-0.5,0.5,-0.5), vec3(0.0,1.0,0.0)},
		{vec3(-0.5,0.5, 0.0), vec3(0.0,1.0,0.5)}
	},
	{
		6,
		0,
		3,
		{vec3( 0.0,0.5,-0.5), vec3(0.5,1.0,0.0)},
		{vec3( 0.0,0.5, 0.0), vec3(0.5,1.0,0.5)}
	}
};

void toImplicit(in vec3 ro, in vec3 rd, out ImplicitRay ir)
{
	ir.o = ro;
	ir.n = 1.0 / rd;
	ir.d = rd;
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
		if((curNode.leafMask & 1) != 0)
		{
			float tTri = hitTriangle(triangles[curNode.childOffset], ro, rd, tNormal, tMax);
			if(tTri > 0)
			{
				tMax = tTri;
				t = tTri;
				normal = tNormal;
			}
		}
		else
		{
			if(hitBox(curNode.AABB1, ir, tNormal, tMax) > 0)
			{
				curNodeNdx += curNode.childOffset;
				continue;
			}
		}

		if((curNode.leafMask & 2) != 0)
		{
			float tTri = hitTriangle(triangles[curNode.childOffset+1], ro, rd, tNormal, tMax);
			if(tTri > 0)
			{
				tMax = tTri;
				t = tTri;
				normal = tNormal;
			}
		}
		else
		{
			if(hitBox(curNode.AABB2, ir, tNormal, tMax) > 0)
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

vec3 sunDir = normalize(vec3(-1.0,4.0,2.0));
vec3 sunLight = 2.0*vec3(1.0,1.0,0.8);
vec3 skyColor(vec3 dir)
{
	return 2*mix(1*vec3(0.1, 0.4, 0.80), 2*vec3(0.3,0.7,1.0), max(0.0,dot(normalize(dir),sunDir)));
}

float hit(in vec3 ro, in vec3 rd, out vec3 normal, float tMax)
{
	return hitBVH(ro, rd, normal, tMax);
}

float hit_any(in vec3 ro, in vec3 rd, float tMax)
{
	/*if(rd.y <= 0.0) // Hit plane
	{
		return 1.0;
	}*/
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