#ifdef PXL_SHADER

// Material
layout(location = 11) uniform sampler2D albedo;

vec3 shadeSurface(ShadeInput inputs)
{
	return texture(albedo, vTexCoord).xyz;
}

#endif // PXL_SHADER