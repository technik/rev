// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Input uniforms
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform mat4 uView;
layout(location = 8) uniform sampler2D uTexturePack;

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

float hitBox(in Box b, in ImplicitRay r, out vec3 normal, float tMax, float tout)
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
		tout = minLeave;
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

float hitSphere(in vec4 sphere, in vec3 ro, in vec3 rd, out vec3 normal, out vec3 albedo, float tMax)
{
	vec3 rRo = ro - sphere.xyz;
	float b = 2*dot(rRo,rd);
	float c = dot(rRo,rRo) - sphere.w*sphere.w;
	float det = b*b-4*c;
	if(det >= 0)
	{
		float t = (-b-sqrt(det))/2;
		normal = normalize(rRo + t* rd);
		albedo = vec3(1.0);
		if (t < tMax)
			return t;
	}
	return -1.0;
}

vec3 skyColor(vec3 dir)
{
	return 2*mix(vec3(0.0045, 0.638, 1.0), vec3(0.95,0.95,1.0), dir.y);
}

struct Node
{
	// top 16 bits: child offset
	// middle 8 bits: leaf mask
	// bottom 8 bits: valid mask
	int descriptor;
	int childOffset[8];
};

Node tree[9] =
{
	{ 0xfdfd , {1,2,3,4,5,6,7,8}},
	{ 0xff5f , {1,2,3,4,5,6,6,7}},
	{ 0xff0b , {1,2,3,4,5,6,6,7}},
	{ 0xff93 , {1,2,3,4,5,6,6,7}},
	{ 0xff63 , {1,2,3,4,5,6,6,7}},
	{ 0xfff5 , {1,2,3,4,5,6,6,7}},
	{ 0xffb0 , {1,2,3,4,5,6,6,7}},
	{ 0xff39 , {1,2,3,4,5,6,6,7}},
	{ 0xff36 , {1,2,3,4,5,6,6,7}},
};


void toImplicit(in vec3 ro, in vec3 rd, out ImplicitRay ir)
{
	ir.o = ro;
	ir.n = vec3(1.0) / rd;
	if(rd.x == 0.0)
		ir.n.x = 100000.0;
	if(rd.y == 0.0)
		ir.n.y = 100000.0;
	if(rd.z == 0.0)
		ir.n.z = 100000.0;
	ir.d = rd;
}

bool voxelExists(int parentNdx, int childNdx)
{
	return (tree[parentNdx].descriptor & childNdx) != 0;
}

void stepRay(in ivec3 oldPos, in int scale, in ImplicitRay ray, out ivec3 pos, out int idx)
{}

float hitOctree(in ImplicitRay ir, out vec3 normal, float tMax)
{
	const int MAX_DEPTH = 2;

	Box rootBox = { vec3(-2.0, 0.0, -4.0), vec3(2.0, 4.0, 0.0) };

	// Find first child
	vec3 t1p = (rootBox.min - ir.o) * ir.n;
	vec3 t2p = (rootBox.max - ir.o) * ir.n;
	// Swapping the order of comparison is important because of NaN behavior
	vec3 tEnter = min(t2p,t1p); // Enters
	vec3 tExit = max(t1p,t2p); // Enters

	// Time of first intersection with the octree
	// Initialize t to the first intersection
	float t = max(max(max(0.0,tEnter.x),tEnter.y),tEnter.z); // If nan, return first operand, which is never nan
	float tExitOctree = min(min(min(tMax,tExit.x),tExit.y),tExit.z); // If nan, return first operand, which is never nan
	if(tExitOctree < t)
		return -1.0; // Skip everything

	vec3 p0 = ir.o + t * ir.d;
	vec3 center = mix(rootBox.min, rootBox.max, 0.5);
	int childNdx = (p0.x > center.x ? 4 : 0) | (p0.y > center.y ? 2:0) | (p0.z > center.z ? 1:0); // Initialize childNdx to the contact point

	vec3 tcross = mix(tEnter, tExit, 0.5);
	int octantMask = (ir.d.x < 0 ? 4 : 0) | (ir.d.y < 0 ? 2 : 0) | (ir.d.z < 0 ? 1 : 0);

	float tExitLevel = tExitOctree;
	int depth = 1;
	int parentNode = 0;

	int nodeStack[MAX_DEPTH];
	nodeStack[0] = parentNode;

	while(t < tExitOctree)
	{
		if(voxelExists(parentNode,(1<<childNdx)))
		{
			// Find hit plane from t
			int orderedNdx = childNdx ^ octantMask;
			tEnter = vec3((orderedNdx&4)!=0?tcross.x:tEnter.x, (orderedNdx&2)!=0?tcross.y:tEnter.y, (orderedNdx&1)!=0?tcross.z:tEnter.z);
			// Should actually push one level here, unless we're at leaf level
			if(depth < MAX_DEPTH)
			{
				depth += 1;
				// Compute child ndx
				int childNode = parentNode + childNdx;
				nodeStack[depth] = childNode;
				//tExitLevel = min(tExitLevel,min(tcross.x,min(tcross.y,tcross.z)));
				//tcross = min(tEnter,tExit,0.5);
				//childNdx = (t > tcross.x ? 4 : 0) | (t > tcross.y ? 2:0) | (t > tcross.z ? 1:0);
				// childNdx ^= octantMask;

			}
			// Compute normal
			normal = vec3(0.0, 0.0, 1.0);
			if(t == tEnter.x)
				normal = vec3(1.0, 0.0, .0);
			else if(t == tEnter.y)
				normal = vec3(0.0, 1.0, 0.0);
			if(dot(normal,ir.d) > 0)
				normal = -normal;
			return t;
		}
		else
		{
			// Figure out next event
			vec3 tEvent = max(vec3(t), tcross);
			float nextEvent = tExitOctree;
			if(tEvent.x > t)
				nextEvent = tEvent.x;
			if(tEvent.y > t)
				nextEvent = min(tEvent.y, nextEvent);
			if(tEvent.z > t)
				nextEvent = min(tEvent.z, nextEvent);
			if(nextEvent >= tExitOctree) // No more internal planes to cross
			{
				//normal = vec3(1.0,0.0,1.0);
				// Should actually pop one level here
				t = tExitLevel;
				return -1.0;
			}
			else // Advance to sibling voxel
			{
				t = nextEvent;
				if(t == tEvent.x)
					childNdx ^= 4;
				if(t == tEvent.y)
					childNdx ^= 2;
				if(t == tEvent.z)
					childNdx ^= 1;
			}
		}
	}

	return t;
}


float hit(in vec3 ro, in vec3 rd, out vec3 normal, out vec3 albedo, float tMax)
{
	float t = -1.0;
	if(rd.y < 0.0) // Hit plane
	{
		normal = vec3(0.0,1.0,0.0);
		t = -ro.y / rd.y;
		albedo = vec3(0.85, 0.5, 0.4);
		tMax = t;
	}
	
	// Convert ray to its implicit representation
	ImplicitRay ir;
	toImplicit(ro,rd,ir);

	// Hit octree
	{
		vec3 tNormal;
		float tOctree = hitOctree(ir, tNormal, tMax);
		if(tOctree >= 0)
		{
			albedo = vec3(0.8);
			normal = tNormal;
			t = tOctree;
			tMax = tOctree;
		}
	}
	return t;
}

float hit_any(in vec3 ro, in vec3 rd, float tMax)
{
	if(rd.y < 0.0) // Hit plane
	{
		return 1.0;
	}
	
	// Convert ray to its implicit representation
	ImplicitRay ir;
	toImplicit(ro,rd,ir);

	// Hit octree
	{
		vec3 tNormal;
		float tOctree = hitOctree(ir, tNormal, tMax);
		if(tOctree >= 0)
		{
			return tOctree;
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
	float z = 2*(seed.y+seed.y/255.0)-1;
	float horRad = sqrt(1-z*z);
	return vec3(
		cos(theta)*horRad,
		sin(theta)*horRad,
		z
		);
}

vec3 lambertianDirection(in vec3 normal, in vec2 seed)
{
	vec3 tangent, bitangent;
	branchlessONB(normal, tangent, bitangent);
	float theta = TwoPi*seed.x;
	float z = cos(HalfPi*seed.y);
	float horRad = sqrt(1-z*z);
	vec3 n = vec3(
		cos(theta)*horRad,
		sin(theta)*horRad,
		z
		);

	return tangent * n.x + bitangent * n.y + normal * n.z;
}