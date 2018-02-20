#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 tangent;
layout(location = 2) in vec3 bitangent;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec2 texCoord;

layout(location = 0) uniform mat4 uWorldViewProjection;
layout(location = 1) uniform vec3 uMSViewPos; // Direction toward viewpoint

out vec3 vtxNormal;
out vec3 vtxViewDir;
out vec2 vTexCoord;

//------------------------------------------------------------------------------
void main ( void )
{
	vTexCoord = texCoord;
	vtxNormal = normal;
	vtxViewDir = uMSViewPos - vertex;
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;
in vec3 vtxNormal;
in vec3 vtxViewDir;
in vec2 vTexCoord;

// Global state
layout(location = 2) uniform vec3 uMSLightDir; // Direction toward light
layout(location = 3) uniform vec3 lightColor;
layout(location = 4) uniform float ev;

float PI = 3.14159265359;

struct ShadeInput
{
	float ndl; // Normal dot light
	float ndv; // Normal dot view
	float ndh; // Normal dot half-vector
};

vec3 shadeSurface(ShadeInput inputs);

//------------------------------------------------------------------------------	
void main (void) {
	// Normalize data from vertex
	vec3 msNormal = normalize(vtxNormal);
	vec3 msLightDir = normalize(uMSLightDir);
	vec3 msViewDir = normalize(vtxViewDir);
	
	ShadeInput shadingInputs;
	//shadingInputs.ndl = dot(msLightDir,msNormal);
	shadingInputs.ndl = max(1e-8,dot(msLightDir,msNormal));
	float ndv = dot(msViewDir,msNormal);
	if(ndv < 0.0)
		msViewDir = reflect(msViewDir, msNormal);
	shadingInputs.ndv = max(1e-8,dot(msViewDir,msNormal));
	
	// Compute illumination intermediate variables
	vec3 msHalfV = normalize(msViewDir+msLightDir);
	shadingInputs.ndh = max(1e-8,dot(msHalfV,msNormal));
	
	// Compute actual lighting
	
	vec3 pbrColor = shadeSurface(shadingInputs);
	// Tone mapping
	pbrColor *= ev;
	//pbrColor = pbrColor / (vec3(1.0) + pbrColor);
	outColor = pow(pbrColor, vec3(2.2));
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif
