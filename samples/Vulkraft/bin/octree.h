


struct Node
{
	// top 16 bits: child offset
	// middle 8 bits: leaf mask
	// bottom 8 bits: valid mask
	int descriptor;
};

bool voxelExists(int parentNdx, int childNdx)
{
	return (sbVoxelOctree.descriptor[parentNdx] & (1<<childNdx)) != 0;
}

//bool isLeaf(int parentNdx, int childNdx)
//{
//	return (sbVoxelOctree.descriptor[parentNdx] & (0x1<<(childNdx+8))) != 0;
//}

int findFirstChild(in vec3 tCross, int octantMask, double t)
{
	int crossedPlanesMask = (t>tCross.x? 4 : 0) | (t > tCross.y? 2 : 0) | (t > tCross.z? 1 : 0);
	return crossedPlanesMask ^ octantMask;
}

int childNode(int parentNode, int childNdx)
{
	int parentDesc = sbVoxelOctree.descriptor[parentNode];
	int firstChildOffet = 1+(parentDesc>>8);
	for(int i = 0; i < childNdx; ++i)
		if((parentDesc & (1<<i)) != 0)
			++firstChildOffet;
	return parentNode + firstChildOffet;
}

float hitOctree(in ImplicitRay ir, out vec3 normal, float tMax)
{
	const int MAX_DEPTH = 5;

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
			if(depth == MAX_DEPTH-1)
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
				int colorval = parentNode;

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