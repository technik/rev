#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 0) uniform mat4 uWorldViewProjection;

out vec3 vtxColor;			

void main ( void )
{
	vtxColor = normal;
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 color;
in vec3 vtxColor;			
void main (void) {
	float light = max(0.0,dot(normalize(vec3(0.2, -0.3, 1.0)),normalize(vtxColor)));
	color = light * vec3(255.0/255.0,22.0/255.0,88.0/255.0) + 0.2*vec3(89.0/255.0,235.0/255.0,1.0);
}
#endif