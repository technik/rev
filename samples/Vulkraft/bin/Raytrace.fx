// Local work group
layout(local_size_x = 1, local_size_y = 1) in;

// Input uniforms
layout(location = 1) uniform vec4 uWindow;

// Output texture
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

vec3 skyColor(vec3 dir)
{
	return mix(vec3(0.6, 0.7, 0.8), vec3(0.95), dir.z);
}

float hit(in vec3 ro, in vec3 rd, out vec3 normal, out vec3 albedo, float tMax)
{
	float t = -1.0;
	if(rd.z < -0.0001) // Hit plane
	{
		normal = vec3(0.0,0.0,1.0);
		t = -ro.z / rd.z;
		albedo = vec3(0.4);
		tMax = t;
	}
	// Hit box
	{
		vec3 boxMin = vec3(-3.0, -1.0, 0.0);
		vec3 boxMax = vec3(-1.0, 1.0, 2.0);
		vec3 rn = vec3(1.0) / rd;
		vec3 t1 = (boxMin - ro) * rn;
		vec3 t2 = (boxMax - ro) * rn;
		// Swapping the order of comparison is important because of NaN behavior
		vec3 tEnter = min(t2,t1); // Enters
		vec3 tExit = max(t1,t2); // Exits
		float maxEnter = max(tEnter.x,max(tEnter.y,max(tEnter.z,0.0))); // If nan, return second operand, which is never nan
		float minLeave = min(tExit.x,min(tExit.y,min(tExit.z,tMax))); // If nan, return second operand, which is never nan
		if(minLeave >= maxEnter)
		{
			t = maxEnter;
			vec3 center = (boxMin + boxMax) * 0.5;
			vec3 dif = t * rd + ro - center;
			vec3 absDif = abs(dif);
			float maxDif = max(absDif.x, max(absDif.y, absDif.z));
			if(maxDif == absDif.x)
			{
				normal = -normalize(vec3(dif.x, 0.0, 0.0));
			}
			else if(maxDif == absDif.y)
			{
				normal = -normalize(vec3(0.0, dif.y, 0.0));
			}
			else if(maxDif == absDif.z)
			{
				normal = -normalize(vec3(0.0, 0.0, dif.z));
			}
			albedo = vec3(0.7);
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
	for(int i = 0; i < 2; ++i)
	{
		vec3 albedo;
		float t = hit(ro,rd, normal, albedo, tMax);
		if(t > 0.0)
		{
			atten *= albedo;
			light += atten * skyColor(normal);
			ro = ro + rd * t + 0.0001 * normal;
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
	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
  
	//
	// Compute uvs
	vec2 uvs = vec2(pixel_coords.x, pixel_coords.y) * (2/uWindow.y) - vec2(uWindow.x/uWindow.y, 1);
	vec3 rd = normalize(vec3(uvs.x, 1.0, uvs.y));
	vec3 ro = vec3(0,-5.0,1.7);

	pixel.xyz = color(ro, rd);
  
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}