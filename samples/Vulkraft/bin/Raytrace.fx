// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Input uniforms
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

const float HalfPi = 1.57079637050628662109375f;
const float TwoPi = 6.2831852436065673828125f;

vec3 skyColor(vec3 dir)
{
	return 2*mix(vec3(0.5, 0.7, 0.85), vec3(0.95), dir.y);
}

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

float hitBox(in Box b, in ImplicitRay r, out vec3 normal, out vec3 albedo, float tMax)
{
	float t = -1.0;
	vec3 t1 = (b.min - r.o) * r.n;
	vec3 t2 = (b.max - r.o) * r.n;
	// Swapping the order of comparison is important because of NaN behavior
	vec3 tEnter = min(t2,t1); // Enters
	vec3 tExit = max(t1,t2); // Exits
	float maxEnter = max(tEnter.x,max(tEnter.y,max(tEnter.z,0.0))); // If nan, return second operand, which is never nan
	float minLeave = min(tExit.x,min(tExit.y,min(tExit.z,tMax))); // If nan, return second operand, which is never nan
	if(minLeave >= maxEnter)
	{
		t = maxEnter;
		vec3 center = (b.min + b.max) * 0.5;
		vec3 dif = t * r.d + r.o - center;
		vec3 absDif = abs(dif);
		float maxDif = max(absDif.x, max(absDif.y, absDif.z));
		if(maxDif == absDif.x)
		{
			normal = normalize(vec3(dif.x, 0.0, 0.0));
		}
		else if(maxDif == absDif.y)
		{
			normal = normalize(vec3(0.0, dif.y, 0.0));
		}
		else
		{
			normal = normalize(vec3(0.0, 0.0, dif.z));
		}
		albedo = vec3(0.35, 0.5, 0.35);
	}
	return t;
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

Box boxes[6] = 
{
	{ vec3(-1.0, 0.0, -5.0), vec3(0.0, 1.0, -4.0) },
	{ vec3(-1.0, 1.0, -4.0), vec3(0.0, 2.0, -3.0) },
	{ vec3(1.0, 0.0, -4.0), vec3(2.0, 1.0, -3.0) },
	{ vec3(-1.0, 0.0, -3.0), vec3(0.0, 1.0, -2.0) },
	{ vec3(0.0, 0.0, -4.0), vec3(1.0, 1.0, -3.0) },
	{ vec3(0.0, 1.0, -4.0), vec3(1.0, 2.0, -3.0) }
};

struct Node
{
	int childMask; // 1: Child node: 0: Child leaf
	int offset[8];
};

Node octree[2] = {
	{ 0xff, {1,0,0,1,0,1,1,0} },
	{ 0xff, {0,0,0,0,0,0,0,0} }
};

Box octreeBBox = { vec3(0.0, 0.0, -7.0), vec3(2.0, 2.0, -5.0) };

float hitSubtree(
	in Box treeBB,
	in ImplicitRay r,
	out vec3 normal,
	out vec3 albedo,
	float tMax)
{
	float t = -1.0;
	
	return t;
}

float hit(in vec3 ro, in vec3 rd, out vec3 normal, out vec3 albedo, float tMax)
{
	float t = -1.0;
	if(rd.y < -0.0001) // Hit plane
	{
		normal = vec3(0.0,1.0,0.0);
		t = -ro.y / rd.y;
		albedo = vec3(0.85, 0.5, 0.4);
		tMax = t;
	}
	// Hit box
	vec3 tNormal;
	vec3 tAlbedo;
	ImplicitRay ir;
	ir.o = ro;
	ir.n = vec3(1.0) / rd;
	ir.d = rd;
	for(int i = 0; i < 6; ++i)
	{
		float tBox = hitBox(boxes[i], ir, tNormal, tAlbedo, tMax);
		if(tBox > 0)
		{
			albedo = tAlbedo;
			normal = tNormal;
			t = tBox;
			tMax = t;
		}
	}
	vec3 sAlbedo;
	vec3 sNormal;
	//float tS = hitBox(boxes[0], ir, tNormal, tAlbedo, tMax);
	/*float tS = hitSphere(vec4(0.0,0.0,0.0,2.0), ro, normalize(rd), sNormal, sAlbedo, tMax);
	if(tS > 0.0)
	{
		albedo = sAlbedo;
		normal = sNormal;
		t = tS;
	}*/
	/*float tT = hitSubtree(octreeBBox, ir, sNormal, sAlbedo, tMax);
	if(tT > 0.0)
	{
		albedo = sAlbedo;
		normal = sNormal;
		t = tT;
	}*/
	return t;
}

// Pixar's method for orthonormal basis generation
void branchlessONB(in vec3 n, out vec3 b1, out vec3 b2)
{
	float sign = n.z > 0.0 ? 1.0 : 0.0;
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
	float cosPhi = 2*seed.y-1;
	float sinPhi = sqrt(1-cosPhi*cosPhi);
	return vec3(
		cos(theta)*sinPhi,
		cosPhi,
		sin(theta)*sinPhi
		);
}

vec3 lambertianDirection(in vec3 normal, in vec2 seed)
{
	vec3 tangent, bitangent;
	onb(normal, tangent, bitangent);
	float t = cos(seed.x*TwoPi*0.5);
	float b = cos(seed.y*TwoPi*0.5);
	float z = sqrt(1-b*b-t*t);

	return tangent * t + bitangent * b + normal * z;
}

vec3 color(vec3 ro, vec3 rd, out float tOut)
{
	vec3 normal;
	vec3 atten = vec3(1.0);
	vec3 light = vec3(0.0);
	float tMax = 100.0;
	tOut = tMax;
	for(int i = 0; i < 4; ++i)
	{
		vec3 albedo;
		float t = hit(ro, rd, normal, albedo, tMax);
		if(t > 0.0)
		{
			if(i == 0)
				tOut = t;
			atten *= albedo;
			//light += atten * skyColor(normal);
			ro = ro + rd * t + 0.0001 * normal;
			// Scatter reflected light
			float noiseX = float((gl_GlobalInvocationID.x + i) % 64) / 64;
			float noiseY = float((gl_GlobalInvocationID.y + (i>>1)) % 64) / 64;
			vec4 noise = texture(uNoise, vec2(noiseX, noiseY));
			if(noise.x > 0.15)
			{
				rd = lambertianDirection(normal, noise.yz);
				if(dot(rd,normal) < 0.0)
					rd = -rd;
			}
			else
				rd = reflect(rd, normal);
		}
		else
		{
			// Sky
			light += atten * skyColor(rd);
			break;
		}
	}
	return light;
}

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0);
	
	float noiseX = float((gl_GlobalInvocationID.x) % 64) / 64;
	float noiseY = float((gl_GlobalInvocationID.y) % 64) / 64;
	vec4 noise = texture(uNoise, vec2(noiseX, noiseY));
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);// + noise.xy;
	//
	// Compute uvs
	vec2 uvs = (vec2(pixel_coords.x, pixel_coords.y)+noise.xy) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 rd = (uView * vec4(normalize(vec3(uvs.x, uvs.y, -2.0)), 0.0)).xyz;
	vec3 ro = (uView * vec4(0,0,0,1.0)).xyz;

	// Tonemapping and gamma correction
	float t;
	pixel.xyz = color(ro, rd, t);
	pixel.w = t;

	// TODO: Temporal anti aliasing
  
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}