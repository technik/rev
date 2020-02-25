// Common math

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
	vec3 v[3];
};

float zFromDepthBuffer(mat4 proj, float d)
{
	float A = proj[2][2];
	float B = proj[2][3];
	float C = proj[3][2];
	float z = B / (C*d-A);
	return z;
}

float depthBufferFromZ(mat4 proj, float z)
{
	float A = proj[2][2];
	float B = proj[2][3];
	float C = proj[3][2];
	float d = B/(z*C) + A/C;
	return d;
}