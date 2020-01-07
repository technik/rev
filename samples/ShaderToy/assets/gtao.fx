#ifdef PXL_SHADER

layout(location = 0) uniform vec4 t;
layout(location = 1) uniform vec4 Window;
layout(location = 2) uniform sampler2D BlueNoise;

const float MIN_DISTANCE = 1e-5;
const float EPSILON = 1e-5;
const float MAX_STEPS = 200;

const float Pi = 3.1415927;
const float TwoPi = 6.2831852436065673828125f;
const float vfov = 90 * Pi / 180;
const float vFocal = tan(vfov*0.5);
const float hFocal = vFocal * Window.x / Window.y;

vec4 s1 = vec4(0.0, 10.0, 1.0+t.w, 5.0);
vec4 s2 = vec4(7.0, 10.0, 1.0+t.w, 3.0);
vec3 b1 = vec3(3*sin(t.y), 5.0, 1.0);
vec3 b1s = vec3(1.0, 1.0, 1.0);

// Difference of products with improved precision
// a*b-c*d making use of fma
float DoP(float a, float b, float c, float d)
{
    float cd = c*d;
    float err = fma(-c,d,cd);
    float apxDop = fma(a,b,-cd);
    return apxDop + err;
}

struct Ray
{
    vec3 o;
    vec3 d;
};

struct Box
{
    vec3 min;
    vec3 max;
};

float tBox(in Ray r, in Box b, out vec3 normal, float tMax)
{
    vec3 rn = vec3(1.0) / r.d;
    vec3 t1 = (b.min - r.o) * rn;
    vec3 t2 = (b.max - r.o) * rn;
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

float tSphere(in Ray r, in vec4 s, out vec3 normal)
{
    // norm2(r.o + r.d * t) = R2
    // (r.o + r.d * t)*(r.o + r.d * t) = R2
    // t2 + 2*r.o*r.d*t + r.o2 - R2 = 0
    // a = 1
    // b = 2*r.o*r.d
    vec3 ro = r.o - s.xyz;
    float bH = dot(ro,r.d);
    // c = r.o2-R2
    float negC = fma(s.w,s.w,-dot(ro,ro));
    // t = -b/2 +- sqrt((b/2)2-c)
    float disc = fma(bH,bH,negC);
    float t = -1.0;
    if(disc > 0)
    {
        t = (-bH - sqrt(disc));
        normal = normalize(ro+t*r.d);
    }
    return t;
}

vec4 sphere0 = vec4(0.0, 0.0, -2.3, 1.0);
Box box1 = { vec3(2.0,-2.0,-4.0),vec3(4.0,4.0,-1.75) };
Box box2 = { vec3(-4.0,-4.0,-4.0),vec3(-2.0,4.0,-1.75) };
Box boxFloor = { vec3(-2.5,-2.0,-2.0),vec3(2.5,-1.0,-1.0) };
Box boxWall = { vec3(-2.7,-2.0,-2.0),vec3(2.7,2.0,-4.0) };
vec3 albedo = vec3(1.0);

float trace(in Ray r, out vec3 normal)
{
    const float TMAX = 10000;
    float t = TMAX;
    vec3 tNormal;
    float tS = tSphere(r,sphere0,normal);
    if(tS >= 0)
    {
        t = tS;
    }
    float tP;
    tP = tBox(r,boxWall,tNormal,TMAX);
    if(tP > 0 && tP < t)
    {
        t = tP;
        normal = tNormal;
    }
    tP = tBox(r,boxFloor,tNormal,TMAX);
    if(tP > 0 && tP < t)
    {
        t = tP;
        normal = tNormal;
    }
    tP = tBox(r,box1,tNormal,TMAX);
    if(tP > 0 && tP < t)
    {
        t = tP;
        normal = tNormal;
    }
    tP = tBox(r,box2,tNormal,TMAX);
    if(tP > 0 && tP < t)
    {
        t = tP;
        normal = tNormal;
    }
    return t==TMAX?-1:t;
}

vec3 lightDir = normalize(vec3(0.2,0.3,-1.0));

vec3 ambient(in vec3 dir)
{
    #if 1
    return vec3(1.0);
    #else
    return mix(vec3(0.5,0.7,1.0),vec3(1.0),max(0.0,dir.z));
    #endif
}

Ray rayFromPixel(vec2 pixelPos)
{
    vec2 uv = pixelPos/Window.xy;
    vec2 retinal = vec2(hFocal,vFocal)*(2*uv-1);
    Ray r;
    r.o = vec3(0.0);
    r.d = normalize(vec3(retinal,-1.0));
    return r;
}

vec3 randUnitVec3(vec2 seed)
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

vec2 noise2d(int seed)
{
    vec4 bn = texture(BlueNoise, gl_FragCoord.xy/64);
    vec2 r;
    r.x = bn[seed];
    r.y = bn[(seed+1)%4];
    return r;
}

float cosWeightVisSample(in vec3 pos, in vec3 normal, in vec2 noise)
{
    Ray sampleRay;
    sampleRay.o = pos + 1e-5*normal;
    sampleRay.d = normalize(normal+0.998*randUnitVec3(noise));
    vec3 tNormal;
    float t = trace(sampleRay,tNormal);
    return t<0?1.0:0.0;
}

const float maxWsDistance = 10;

float vsPosFromPixel(vec2 pixelPos, out vec3 vsPos)
{
    Ray r = rayFromPixel(pixelPos);
    vec3 normal;
    float t = trace(r,normal);
    vsPos = t*r.d;
    return t;
}

float horizon(vec3 origin, vec2 ds, vec3 vsNormal)
{
    vec3 occl;
    float t = vsPosFromPixel(gl_FragCoord.xy+ds.xy, occl);
    if(t < 0) // No obstacle traced
        return 1.0;
    vec3 dx = normalize(occl-origin);
    float ndh = dot(dx,vsNormal);
    if(ndh <= 0)
        return 1.0;
    float Snds = dot(vsNormal.xy, ds);
    if(Snds <= 0.0)
    {
        return 1-ndh*ndh;
    }
    else
    {
        float ndz = vsNormal.z;
        float s2 = dot(ds,ds);
        float sinH = -dx.z*Snds+s2*ndz;
        return (sinH<0?-1.0:1.0)*(1-ndh*ndh);
    }
}

float minHorizonVis(vec3 vsCenter, vec2 dx, vec3 vsNormal)
{
    float minVis = 1.0;
    const int nSamples = 64;
    for(int i = 0; i < nSamples; ++i)
    {
        vec2 x1 = dx*(i+1);
        float h = horizon(vsCenter,x1,vsNormal);
        minVis = min(minVis, h);
    }
    return minVis;
}

float sliceGTAO(
    in vec2 ssSampleDir,
    in vec3 vsNormal)
{
    vec3 origin;
    float t = vsPosFromPixel(gl_FragCoord.xy, origin);
    // Same thing with z dir
    vec2 ds = 5.0*ssSampleDir;
    //float maxCosH1 = maxCosHorizon(origin,ds, vsNormal);
    //float maxCosH2 = maxCosHorizon(origin,-ds, vsNormal);
    //float sin2H1 = maxCosH1;//1-maxCosH1*maxCosH1;
    //float sin2H2 = maxCosH2;//1-maxCosH2*maxCosH2;
    float sin2H1 = minHorizonVis(origin, ds, vsNormal);
    float sin2H2 = minHorizonVis(origin,-ds, vsNormal);

    return 0.5*(sin2H1+sin2H2); // Already normalized
}

vec2 upVec2(float u)
{
    float x = u;//2*u-1;
    return vec2(x,sqrt(1-x*x));
}

vec3 gtAO()
{
    Ray r = rayFromPixel(gl_FragCoord.xy);
    vec3 normal;
    float t = trace(r, normal);
    if(t < 0)
        return vec3(1.0);

    vec4 seeds = texelFetch(BlueNoise, ivec2(gl_FragCoord.xy)%64, 0);
    
    float w = 0.0;
    const int nSamples = 1;
    for(int i = 0; i < nSamples; ++i)
    {
        float t = (seeds[i%4]+i)*Pi/nSamples; // Jittered samples
        vec2 ssSampleDir = upVec2(cos(t)); // Random sample direction

        w += sliceGTAO(ssSampleDir, normal);
    }

    return vec3(w/nSamples);
}

vec3 hbAOSample(in Ray r, in vec2 noise)
{
    vec3 normal;
    float t = trace(r, normal);
    if(t > 0)
    {
        r.o = r.o + r.d*t+normal*1e-4;
        r.d = normalize(normal+0.998*randUnitVec3(noise));
        t = trace(r,normal);
        return t<0?vec3(1.0):vec3(0.0);
    }
    else
        return vec3(1.0);
}

vec2 fracNoise(vec2 seed)
{
    vec2 noise;
    noise.x = fract(sin(seed.x+0.213)*1200);
    noise.y = fract(sin(seed.y+0.327)*760);
    return noise;
}

// Monte-carlo integration of cosine weighted visibility term at one point
vec3 monteCarloCosVis(in vec2 uvCenter)
{
    Ray r = rayFromPixel(gl_FragCoord.xy);
    int nSamples = 32;
    vec2 seed = noise2d(0); // Seed[0:1]
    vec3 color = vec3(0.0);
    for(int i = 0; i < nSamples; ++i)
    {
        seed = fracNoise(seed);
        color += hbAOSample(r, seed);
    }
    return vec3(color.r / nSamples);
}

vec3 shade ()
{
	vec2 uv = (2.0*gl_FragCoord.xy / Window.xy)-1.0; // [-1, 1]
    vec3 color = monteCarloCosVis(uv);
    color = gtAO();//-color;
    //return color;
    return abs(color);
    //return (color*0.5+0.5);
	//return pow(color, vec3(0.4545));
}

#endif