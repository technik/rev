// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Input uniforms
layout(location = 1) uniform vec4 uWindow;
layout(location = 2) uniform mat4 uView;
layout(location = 3) uniform sampler2D uNoise;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

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

Box boxes[6] = 
{
	{ vec3(-1.0, 0.0, -4.0), vec3(0.0, 1.0, -3.0) },
	{ vec3(-1.0, 1.0, -4.0), vec3(0.0, 2.0, -3.0) },
	{ vec3(1.0, 0.0, -4.0), vec3(2.0, 1.0, -3.0) },
	{ vec3(-1.0, 0.0, -3.0), vec3(0.0, 1.0, -2.0) },
	{ vec3(0.0, 0.0, -4.0), vec3(1.0, 1.0, -3.0) },
	{ vec3(0.0, 1.0, -4.0), vec3(1.0, 2.0, -3.0) },
};

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
	return t;
}

vec3 color(vec3 ro, vec3 rd)
{
	vec3 normal;
	vec3 atten = vec3(1.0);
	vec3 light = vec3(0.0);
	float tMax = 100.0;
	for(int i = 0; i < 4; ++i)
	{
		vec3 albedo;
		float t = hit(ro,rd, normal, albedo, tMax);
		if(t > 0.0)
		{
			atten *= albedo;
			//light += atten * skyColor(normal);
			ro = ro + rd * t + 0.0001 * normal;
			// Scatter reflected light
			float noiseX = float((gl_GlobalInvocationID.x + i) % 64) / 64;
			float noiseY = float((gl_GlobalInvocationID.y + (i>>1)) % 64) / 64;
			vec4 noise = texture(uNoise, vec2(noiseX, noiseY));
			rd = reflect(rd, normal);
			if(noise.w > 0.15)
			{
				vec3 randomV = noise.xyz * 2 - 1;
				if(dot(randomV,normal) > 0.0)
					rd = randomV;
				else
					rd = -randomV;
			}
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
	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
  
	//
	// Compute uvs
	vec2 uvs = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 rd = (uView * vec4(normalize(vec3(uvs.x, uvs.y, -2.0)), 0.0)).xyz;
	vec3 ro = (uView * vec4(0,0,0,1.0)).xyz;

	// Tonemapping and gamma correction
	pixel.xyz = color(ro, rd);
	pixel.xyz = pixel.xyz / (pixel.xyz + 1);
	//pixel.xyz = pow(pixel.xyz, vec3(0.4545));
  
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}