#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;

layout(std140, binding = 0) buffer world2ShadowProjSSBO
{
	mat4 matrixSSBO[];
};

//layout(location = 0) uniform mat4 world2ShadowProj;
layout(location = 1) uniform float uBaseInstance;

//------------------------------------------------------------------------------
void main ( void )
{
	int instanceId = int(uBaseInstance);
	gl_Position = matrixSSBO[instanceId] * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;

//------------------------------------------------------------------------------	
void main (void) {
	outColor = vec3(1.0);
}

#endif
