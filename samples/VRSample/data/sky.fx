#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 msTangent;
layout(location = 2) in vec3 msBitangent;
layout(location = 3) in vec3 msNormal;
layout(location = 4) in vec2 texCoord;

layout(location = 0) uniform mat4 invViewProj;
layout(location = 1) uniform vec3 uMSViewPos; // Direction toward viewpoint

out vec3 vtxViewDir;

//------------------------------------------------------------------------------
void main ( void )
{
	vtxViewDir = (inverse(invViewProj) * vec4(vertex.xy, 0.0, 1.0)).xyz; // Direction from the view point
	gl_Position = vec4(vertex.xy, 1.0, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;
in vec3 vtxViewDir;

// Global state
layout(location = 4) uniform float ev;

float PI = 3.14159265359;

layout(location = 2) uniform sampler2D hdrSkyTexture;

//------------------------------------------------------------------------------	
void main (void) {
	outColor = pow(normalize(vtxViewDir), vec3(2.2));
}

#endif
