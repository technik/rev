#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 0) uniform mat4 uWorldViewProjection;

out vec3 vtxNormal;			

//------------------------------------------------------------------------------
void main ( void )
{
	vtxNormal = normal;
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;
in vec3 vtxNormal;		

layout(location = 1) uniform vec3 uMSLightDir; // Direction toward light
layout(location = 2) uniform vec3 uMSViewDir; // Direction toward viewpoint

float roughness = 0.2;
float ev = 50;
vec3 lightColor = 100.0 * vec3(1.0, 1.0, 0.9);

float PI = 3.14159265359;

//------------------------------------------------------------------------------
float GGX(float NdH, float a)
{
    float a2     = a*a;
    float NdH2 = NdH*NdH;
	
    float nom    = a2;
    float denom  = (NdH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

//------------------------------------------------------------------------------	
void main (void) {
	// Normalize data from vertex
	vec3 msNormal = normalize(vtxNormal);
	vec3 msLightDir = normalize(uMSLightDir);
	vec3 msViewDir = normalize(uMSViewDir);
	
	// Compute illumination intermediate variables
	vec3 msHalfV = normalize(msViewDir+msLightDir);
	float NdL = max(0.0,dot(msLightDir,msNormal));
	float NdH = max(0.0,dot(msHalfV,msNormal));
	
	// Compute actual lighting
	float light = NdL*(1.0-NdH) + NdH;
	outColor = light * lightColor;
	// Tone mapping
	outColor = (outColor / ev);
	outColor = outColor / (vec3(1.0) + outColor);
	outColor = pow(outColor, vec3(2.2));
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif