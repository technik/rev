#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 0) uniform mat4 uWorldViewProjection;

out vec3 vtxNormal;			

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
	
void main (void) {
	vec3 msNormal = normalize(vtxNormal);
	vec3 msLightDir = normalize(uMSLightDir);
	vec3 msViewDir = normalize(uMSViewDir);
	vec3 msHalfV = normalize(msViewDir+msLightDir);
	float NdL = max(0.0,dot(msLightDir,msNormal));
	float NdH = max(0.0,dot(msHalfV,msNormal));
	float light = NdL*(1.0-NdH) + NdH;
	outColor = light * vec3(1.0);
	outColor = pow(outColor, vec3(2.2));
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif