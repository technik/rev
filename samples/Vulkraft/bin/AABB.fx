// AABB
#ifndef _AABB_FX_
#define _AABB_FX_
#include "ImplicitRay.fx"

struct Box
{
	vec3 min;
	vec3 max;
};

float hitBox(in Box b, in ImplicitRay r, out vec3 normal, float tMax)
{
	float dontOptimizeNan = 0.0000000000000000000000000001;
	vec3 t1 = (b.min - r.o+dontOptimizeNan) * r.n;
	vec3 t2 = (b.max - r.o+dontOptimizeNan) * r.n;
	// Swapping the order of comparison is important because of NaN behavior
	vec3 tEnter = min(t1,t2); // Enters
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

#endif // _AABB_FX_