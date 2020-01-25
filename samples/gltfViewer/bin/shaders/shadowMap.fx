#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;

/*layout(std140, binding = 0) uniform world2ShadowProj
{
	mat4 matrix;
}*/

layout(location = 0) uniform mat4 world2ShadowProj;

//------------------------------------------------------------------------------
void main ( void )
{
	gl_Position = world2ShadowProj * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;

//------------------------------------------------------------------------------	
void main (void) {
	outColor = vec3(1.0);
}

#endif
