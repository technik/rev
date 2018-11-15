#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 0) uniform mat4 wvp;

//------------------------------------------------------------------------------
void main ( void )
{
	gl_Position = wvp * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;

//------------------------------------------------------------------------------	
void main (void) {
	outColor = vec3(1.0);
}

#endif
