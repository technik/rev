#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;

layout(location = 0) uniform mat4 invViewProj;

out vec3 vtxViewDir;

//------------------------------------------------------------------------------
void main ( void )
{
	vtxViewDir = (inverse(invViewProj) * vec4(vertex.xy, -1.0, 1.0)).xyz; // Direction from the view point
	gl_Position = vec4(vertex.xy, -1.0, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;
in vec3 vtxViewDir;

// Global state
layout(location = 3) uniform float uEV;
layout(location = 7) uniform sampler2D hdrSkyTexture;

float PI = 3.14159265359;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSpherical(vec3 v)
{
	vec2 uv = vec2(atan(v.y, -v.x), asin(-v.z));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//------------------------------------------------------------------------------	
void main (void) {
	vec3 color = textureLod(hdrSkyTexture, sampleSpherical(normalize(vtxViewDir)), 0.0).xyz;
	
	outColor = uEV*color;
	/*if(vtxViewDir.z < 0.0)
		outColor = vec3(0.4);
	if(vtxViewDir.x*vtxViewDir.x+vtxViewDir.z*vtxViewDir.z < 0.008)
		outColor = vec3(1.0);*/
}

#endif
