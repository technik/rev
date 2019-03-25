// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Input uniforms
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform mat4 uCamWorld;
layout(location = 3) uniform mat4 uViewMtx;
layout(location = 4) uniform mat4 uProj;
layout(location = 5) uniform sampler2D uTexturePack;
layout(location = 6) uniform vec4 uNoiseOffset;

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

vec3 worldSpaceRay(mat4 camWorld, vec2 clipSpacePos)
{
	float zClipDepth = 0.0;
	float wClip = uProj[3][2] / (zClipDepth - uProj[2][2]/uProj[2][3]);
	vec3 csPos = vec3(clipSpacePos.x, clipSpacePos.y, zClipDepth);
	vec4 vsPos = (inverse(uProj) * vec4(csPos, 1))*wClip;
	vec4 wsPos = camWorld * vsPos;
	vec4 wsEyePos = camWorld * vec4(vec3(0.0), 1.0);

	vec3 wsEyeDir = normalize(wsEyePos.xyz-wsPos.xyz);
	return -wsEyeDir;
}

#ifdef GBUFFER
vec3 fetchAlbedo(vec3 worldPos, vec3 worldNormal, float t, int lodBias)
{
	//return vec3(0.5);
	// Choose material
	ivec2 tileOffset = ivec2(0,0);
	if(worldPos.y > 0.01)
		tileOffset = ivec2(2,4);
	if(worldPos.y > 1.99)
		tileOffset = ivec2(4,0);
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

	return texelFetch(uTexturePack, ivec2(texUV.x,texUV.y), texLOD).xyz;
}
#endif

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

vec3 sunDir = normalize(vec3(-1.0,4.0,2.0));
vec3 sunLight = 2.0*vec3(1.0,1.0,0.8);
vec3 skyColor(vec3 dir)
{
	return 2*mix(1*vec3(0.1, 0.4, 0.80), 2*vec3(0.3,0.7,1.0), max(0.0,dot(normalize(dir),sunDir)));
}

struct Node
{
	// top 16 bits: child offset
	// middle 8 bits: leaf mask
	// bottom 8 bits: valid mask
	int descriptor;
};

Node tree[5+4*8] =
{
	{ 0<<16 | 0x00<<8 | 0x33 }, // Top level
	{ 3<<16 | 0x00<<8 | 0xff }, // Mid level
	{ 10<<16 | 0x00<<8 | 0xff }, // Mid level
	{ 17<<16 | 0x00<<8 | 0xff }, // Mid level
	{ 24<<16 | 0x00<<8 | 0xff }, // Mid level
	// Cube a
	{ 0xff<<8 | 0x57 },
	{ 0xff<<8 | 0x0b },
	{ 0xff<<8 | 0x93 },
	{ 0xff<<8 | 0xc3 },
	{ 0xff<<8 | 0x55 },
	{ 0xff<<8 | 0x00 },
	{ 0xff<<8 | 0x99 },
	{ 0xff<<8 | 0x4c },
	// Cube b
	{ 0xff<<8 | 0x07 },
	{ 0xff<<8 | 0xab },
	{ 0xff<<8 | 0xc3 },
	{ 0xff<<8 | 0x63 },
	{ 0xff<<8 | 0x00 },
	{ 0xff<<8 | 0x00 },
	{ 0xff<<8 | 0x8c },
	{ 0xff<<8 | 0x66 },
	// Cube c
	{ 0xff<<8 | 0x55 },
	{ 0xff<<8 | 0x00 },
	{ 0xff<<8 | 0x99 },
	{ 0xff<<8 | 0xc4 },
	{ 0xff<<8 | 0x75 },
	{ 0xff<<8 | 0xb0 },
	{ 0xff<<8 | 0x39 },
	{ 0xff<<8 | 0x3c },
	// Cube d
	{ 0xff<<8 | 0x00 },
	{ 0xff<<8 | 0x2a },
	{ 0xff<<8 | 0xc8 },
	{ 0xff<<8 | 0x66 },
	{ 0xff<<8 | 0x70 },
	{ 0xff<<8 | 0xb2 },
	{ 0xff<<8 | 0x3c },
	{ 0xff<<8 | 0x36 },
};
const Box rootBox = { vec3(-4.0, 0.0, -8.0), vec3(4.0, 8.0, 0.0) };

void toImplicit(in vec3 ro, in vec3 rd, out ImplicitRay ir)
{
	ir.o = ro;
	ir.n = 1.0 / rd;
	ir.d = rd;
}

bool voxelExists(int parentNdx, int childNdx)
{
	return (tree[parentNdx].descriptor & (1<<childNdx)) != 0;
}

bool isLeaf(int parentNdx, int childNdx)
{
	return (tree[parentNdx].descriptor & (0x1<<(childNdx+8))) != 0;
}

int findFirstChild(in vec3 tCross, int octantMask, double t)
{
	int crossedPlanesMask = (t>tCross.x? 4 : 0) | (t > tCross.y? 2 : 0) | (t > tCross.z? 1 : 0);
	return crossedPlanesMask ^ octantMask;
}

int childNode(int parentNode, int childNdx)
{
	int firstChildOffet = 1 + (tree[parentNode].descriptor>>16);
	for(int i = 0; i < childNdx; ++i)
		if((tree[parentNode].descriptor & (1<<i)) != 0)
			++firstChildOffet;
	return parentNode + firstChildOffet;
}

float hitOctree(in ImplicitRay ir, out vec3 normal, float tMax)
{
	const int MAX_DEPTH = 2;

	// Find first child
	vec3 t1 = (rootBox.min - ir.o) * ir.n;
	vec3 t2 = (rootBox.max - ir.o) * ir.n;
	// Swapping the order of comparison is important because of NaN behavior
	vec3 tNear = min(t1,t2); // Intersection with the planes that are closer to the ray origin
	vec3 tFar = max(t2,t1); // Intersection with the planes that are further from the ray origin

	// Set t to the first intersection with the cube
	float t = max(max(max(0.0,tNear.x),tNear.y),tNear.z); // If nan, return first operand, which is never nan
	float tExit = min(min(min(tMax,tFar.x),tFar.y),tFar.z); // If nan, return first operand, which is never nan
	if(tExit < t)
		return -1.0; // Box not in range, Skip everything

	int rayDirMask = (ir.d.x<=0?4:0) | (ir.d.y<=0?2:0) | (ir.d.z<=0?1:0);
	vec3 tcross = ((rootBox.min + rootBox.max)*0.5 - ir.o) * ir.n;
	int childNdx = findFirstChild(tcross, rayDirMask, t);
	ivec3 pos = ivec3(childNdx>>2, (childNdx>>1) & 1, childNdx&1);
	vec3 cornerDir = vec3(1-(rayDirMask>>2), 1-(rayDirMask>>1&1), 1-(rayDirMask&1));

	int depth = 0;
	int parentNode = 0;

	// Parent node stack
	int nodeStack[MAX_DEPTH];
	nodeStack[0] = parentNode;
	int eventMask = (t==tNear.x?4:0)|(t==tNear.y?2:0)|(t==tNear.z?1:0);

	while(true)
	{
		if(voxelExists(parentNode,childNdx))
		{
			if(isLeaf(parentNode,childNdx))
			{
				// Compute normal
				normal = vec3(0.0, 0.0, 1.0);
				if((eventMask&4)!=0)
					normal = vec3(1.0, 0.0, .0);
				else if((eventMask&2)!=0)
					normal = vec3(0.0, 1.0, 0.0);
				// Reverse direction
				if(dot(normal,ir.d) > 0)
					normal = -normal;

				return t;
			}
			else
			{	
				// Push one level
				nodeStack[depth] = parentNode;
				parentNode = childNode(parentNode, childNdx);
				++depth;
				// Find first child
				vec3 childSize = (rootBox.max-rootBox.min)*(1.0/(2<<depth));
				vec3 crossPlane = rootBox.min + childSize * (2*vec3(pos)+1);
				tcross = (crossPlane - ir.o) * ir.n;
				childNdx = findFirstChild(tcross, rayDirMask, t);
				pos.x = (pos.x<<1) | (childNdx>>2);
				pos.y = (pos.y<<1) | ((childNdx&2)>>1);
				pos.z = (pos.z<<1) | (childNdx&1);
			}
		}
		else
		{
			// Figure out next event
			vec3 childSize = (rootBox.max-rootBox.min)*(1.0/(2<<depth));
			vec3 nextPlanes = rootBox.min + childSize * (vec3(pos.x,pos.y,pos.z)+cornerDir);
			vec3 tcorner = (nextPlanes - ir.o) * ir.n;

			tExit = min(min(min(tMax,tcorner.x),tcorner.y),tcorner.z);
			if(tExit >= tMax)
				return -1.0;
			// Which planes must me crossed
			eventMask = (tExit==tcorner.x?4:0)|(tExit==tcorner.y?2:0)|(tExit==tcorner.z?1:0);
			// Which planes can be stepped without poping
			int canStep = childNdx^ 7 ^rayDirMask;
			while((canStep&eventMask) != eventMask) // Pop until we can!
			{
				if(depth > 0) // Pop one level
				{
					--depth;
					pos >>= 1;
					childNdx = (pos.x&1)<<2 | (pos.y&1)<<1 | (pos.z&1);
					canStep = childNdx^ 7 ^rayDirMask;

					parentNode = nodeStack[depth];
				}
				else
				{
					return -1.0;
				}
			}

			// Switch to next sibling
			t = tExit;
			if((eventMask&4) != 0)
			{
				childNdx ^= 4;
				pos.x ^= 1;
			}
			if((eventMask&2) != 0)
			{
				childNdx ^= 2;
				pos.y ^= 1;
			}
			if((eventMask&1) != 0)
			{
				childNdx ^= 1;
				pos.z ^= 1;
			}
		}
	}

	return -1.0;
}


float hit(in vec3 ro, in vec3 rd, out vec3 normal, float tMax)
{
	float t = -1.0;
	// Convert ray to its implicit representation
	ImplicitRay ir;
	toImplicit(ro,rd,ir);

	// Hit ground plane
	if(rd.y < 0.0)
	{
		normal = vec3(0.0,1.0,0.0);
		t = -ro.y * ir.n.y;
		tMax = t;
	}
	
	// Hit octree
	{
		vec3 tNormal;
		float tOctree = hitOctree(ir, tNormal, tMax);
		if(tOctree >= 0)
		{
			normal = tNormal;
			t = tOctree;
			tMax = tOctree;
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