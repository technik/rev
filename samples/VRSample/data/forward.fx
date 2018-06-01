#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 msTangent;
layout(location = 2) in vec3 msBitangent;
layout(location = 3) in vec3 msNormal;
layout(location = 4) in vec2 texCoord;

layout(location = 0) uniform mat4 uWorldViewProjection;
layout(location = 1) uniform mat4 uWorld;
//layout(location = 2) uniform mat4 uMs2Shadow;
layout(location = 4) uniform vec3 uWsViewPos; // Direction toward viewpoint

#ifdef sampler2D_uNormalMap
out vec3 vtxTangent;
out vec3 vtxBitangent;
#endif
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
#ifdef sampler2D_uNormalMap
	vtxTangent = worldRot * msTangent;
	vtxBitangent = worldRot * msBitangent;
#endif
	vtxNormal = inverse(transpose(worldRot)) * msNormal;
	// Lighting vectors
	vtxWsPos = (uWorld * vec4(vertex,1.0)).xyz;
	vtxWsEyeDir = uWsViewPos - vtxWsPos;
	gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;
#ifdef sampler2D_uNormalMap
in vec3 vtxTangent;
in vec3 vtxBitangent;
#endif
in vec3 vtxNormal;
in vec3 vtxWsEyeDir;
in vec2 vTexCoord;
in vec3 vtxWsPos;

// Global state
//layout(location = 2) uniform mat4 uMs2Shadow;
layout(location = 3) uniform float uEV;
layout(location = 5) uniform vec3 uLightColor;
layout(location = 6) uniform vec3 uLightDir; // Direction toward light
layout(location = 9) uniform sampler2D uShadowMap;
#ifdef sampler2D_uNormalMap
layout(location = 10) uniform sampler2D uNormalMap;
#endif

const float PI = 3.14159265359;
const float TWO_PI = 6.283185307179586;
const float HALF_PI = 1.5707963267948966;
const float INV_PI = 0.3183098861837907;

struct ShadeInput
{
	float ndv; // Normal dot view
	// World space vectors
#ifdef sampler2D_uNormalMap
	vec3 tangent;
	vec3 bitangent;
#endif
	vec3 normal;
	vec3 eye;
};

vec3 shadeSurface(ShadeInput inputs);

#ifdef sampler2D_uNormalMap
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
#endif

//------------------------------------------------------------------------------	
void main (void) {
	// Build shader inputs
	ShadeInput shadingInputs;
	// Tangent space
#ifdef sampler2D_uNormalMap
	vec3 tangent = normalize(vtxTangent);
	vec3 bitangent = normalize(vtxBitangent);
	vec3 normal = getSampledNormal(tangent, bitangent, normalize(vtxNormal));
#else
	vec3 normal = normalize(vtxNormal);
#endif
#ifdef sampler2D_uNormalMap
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

	shadingInputs.tangent = tangent;
	shadingInputs.bitangent = bitangent;
#endif
	shadingInputs.normal = normal;

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
	
	//#ifdef ANDROID
	outColor = pbrColor*uEV;
	//#else
	//outColor = pow(pbrColor*uEV, vec3(2.2));
	//#endif
	//outColor = 0.5+0.5*msNormal;//pow(pbrColor, vec3(2.2));
}

// rosa vec3(255.0/255.0,22.0/255.0,88.0/255.0)
// turquesa vec3(89.0/255.0,235.0/255.0,1.0)
#endif
