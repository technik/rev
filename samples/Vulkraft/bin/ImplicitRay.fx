// Implicit ray convsersions
#ifndef _IMPLICITRAY_FX_
#define _IMPLICITRAY_FX_

struct ImplicitRay
{
	vec3 o;
	vec3 n;
	vec3 d;
};

void toImplicit(in vec3 ro, in vec3 rd, out ImplicitRay ir)
{
	ir.o = ro;
	ir.n = 1.0 / rd;
	ir.d = rd;
}

#endif // _IMPLICITRAY_FX_