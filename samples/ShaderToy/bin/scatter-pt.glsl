#ifdef PXL_SHADER

layout(location = 0) uniform vec4 t;
layout(location = 1) uniform vec4 Window;

#define PI 3.1415927
#define earthRadius 6378000.0

const float SunAlt = PI/6.0;
const float CamAlt = 0.0*PI/8.0;
const vec3 beta0 = vec3(0.000005197, 0.00001214, 0.00002965);

vec3 sunDir = vec3(0.0, sin(SunAlt-CamAlt), -cos(SunAlt-CamAlt));

vec3 sunDisk(vec3 dir)
{
    float cosArc = dot(dir, sunDir);
    vec3 col = vec3(cosArc > 0.99998917 ? 1.0 : 0.0);
    return col;
}

float atmosphereDepth(vec3 dir)
{
    float sinTheta = dir.y;
    float b = 2.0*earthRadius*sinTheta;
    const float h = 100000.0;
    float c = -h*(h+2.0*earthRadius);
    return (-b+sqrt(b*b-4.0*c))*0.5;
}

float airDensity(float h)
{
    return exp(-h/8000.0);
}

float linearAltitude(float sinTheta, float d)
{
    return sqrt(d*d + earthRadius*earthRadius + 2.0*d*sinTheta*earthRadius)-earthRadius;
}

#define NSamples 20.0
float opticalDepth(float sinTheta, float d)
{
	float s = d/(NSamples-1.0);
	float depth = 0.5*s*(airDensity(0.0)+airDensity(linearAltitude(sinTheta, d)));
	for(float i = 0.0; i < NSamples-2.0; ++i)
    {
		float h = linearAltitude(sinTheta, (i+1.0)*s);
		depth += s*airDensity(h);
    }
	return depth;
}

vec3 sunLight(vec3 dir)
{
    return sunDisk(dir) * exp(-beta0*opticalDepth(dir.y, 100000.0));
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    float aspectRatio = Window.x/Window.y;
    float vFov = PI/2.05;
    vec2 uv = (fragCoord/Window.xy*2.0-1.0)*vec2(aspectRatio, 1)*tan(vFov/2.0);
    vec3 rd = normalize(vec3(uv.x,uv.y,-1));

    // Time varying pixel color
    vec3 col = sunLight(rd);

    // Output to screen
    fragColor = vec4(pow(col,vec3(0.4545)),1.0);
}

vec3 shade () {	
	vec2 uv = gl_FragCoord.xy / Window.xy;

	// TODO: Pathtrace the scene

	// TODO: Tonemapping
	// TODO: Gamma correction
	return vec3(uv.x,uv.y,1.0*sin(t.y));
}

#endif