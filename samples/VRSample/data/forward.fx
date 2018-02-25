#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 msTangent;
layout(location = 2) in vec3 msBitangent;
layout(location = 3) in vec3 msNormal;
layout(location = 4) in vec2 texCoord;

layout(location = 0) uniform mat4 uWorldViewProjection;
layout(location = 1) uniform mat4 uWorld;
layout(location = 2) uniform mat4 uMs2Shadow;
layout(location = 4) uniform vec3 uWsViewPos; // Direction toward viewpoint

out vec3 vtxTangent;
out vec3 vtxBitangent;
out vec3 vtxNormal;
out vec3 vtxWsEyeDir;
out vec2 vTexCoord;
out vec3 vtxWsPos;

//------------------------------------------------------------------------------
void main ( void )
{
	// Textures
	vTexCoord = texCoord;
	// Tangent space
	mat3 worldRot = mat3(uWorld);
	vtxTangent = worldRot * msTangent;
	vtxBitangent = worldRot * msBitangent;
	vtxNormal = worldRot * msNormal;
	// Lighting vectors
	vtxWsPos = (uWorld * vec4(vertex,1.0)).xyz;
	vtxWsEyeDir = uWsViewPos - vtxWsPos;
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;
in vec3 vtxTangent;
in vec3 vtxBitangent;
in vec3 vtxNormal;
in vec3 vtxWsEyeDir;
in vec2 vTexCoord;
in vec3 vtxWsPos;

// Global state
layout(location = 2) uniform mat4 uMs2Shadow;
layout(location = 3) uniform float uEV;
layout(location = 5) uniform vec3 uLightColor;
layout(location = 6) uniform vec3 uLightDir; // Direction toward light
layout(location = 9) uniform sampler2D uShadowMap;
layout(location = 10) uniform sampler2D uNormalMap;

float PI = 3.14159265359;

struct ShadeInput
{
	float ndv; // Normal dot view
	// World space vectors
	vec3 tangent;
	vec3 bitangent;
	vec3 normal;
	vec3 eye;
};

vec3 shadeSurface(ShadeInput inputs);

//------------------------------------------------------------------------------
vec3 getSampledNormal(vec3 tangent, vec3 bitangent, vec3 normal)
{
	vec3 texNormal = (255.f/128.f)*texture(uNormalMap, vTexCoord).xyz - 1.0;
	
	//return normal;
	return normalize(
		tangent*texNormal.x +
		bitangent*texNormal.y +
		normal*max(texNormal.z, 1e-8)
	);
}

//------------------------------------------------------------------------------	
void main (void) {
	// Build shader inputs
	ShadeInput shadingInputs;
	// Tangent space
	vec3 tangent = normalize(vtxTangent);
	vec3 bitangent = normalize(vtxBitangent);
	vec3 normal = getSampledNormal(tangent, bitangent, normalize(vtxNormal));
	// Recompute tangent space around sampled normal
	tangent = normalize(
		tangent
		-dot(normal,tangent)*normal
		);
	bitangent = normalize(
		bitangent
		-dot(normal,bitangent)*normal
		-dot(tangent,bitangent)*tangent
		);

	shadingInputs.normal = normal;
	shadingInputs.tangent = tangent;
	shadingInputs.bitangent = bitangent;

	shadingInputs.eye = normalize(vtxWsEyeDir);
	float ndv = dot(shadingInputs.eye,shadingInputs.normal);
	if(ndv < 0.0)
		shadingInputs.eye = reflect(shadingInputs.eye, shadingInputs.normal);
	shadingInputs.ndv = max(1e-8,dot(shadingInputs.eye,shadingInputs.normal));

	// ---- Shading ----
	// Compute actual lighting
	vec3 pbrColor = shadeSurface(shadingInputs);
	// Tone mapping
	//pbrColor = pbrColor / (1.0 + pbrColor);
	
	//outColor = pbrColor;
	//outColor = pow(pbrColor*uEV, vec3(4.4));
	outColor = pow(pbrColor*uEV, vec3(2.2));
	//outColor = pbrColor*uEV;
	//outColor = 0.5+0.5*msNormal;//pow(pbrColor, vec3(2.2));
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif
