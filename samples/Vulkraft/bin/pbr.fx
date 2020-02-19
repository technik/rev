// PBR shading

// Approximated correlated visibility G1 term
float smithVisibilityG1(float k, float ndv)
{
	return ndv / (ndv*(1-k)+k);
}

// Approximated correlated visibility G2 term
// This is predivided by 4*ndl*ndv to avoid that division in the brdf computation
float smithVisibilityG2(float alpha, float ndl, float ndv)
{
	float den = 2*mix(2*ndl*ndv, ndl+ndv, alpha);
	return 1 / den;
}

float geometricDistribution(float alpha, float ndh)
{
	float a2 = alpha*alpha;
	float den = ndh*ndh*(a2-1)+1;
	return a2/(den*den);
}

float specBRDF(float roughness, float ndh, float ndv, float ndl)
{
	float alpha = roughness*roughness;

	// Normalized normal distribution
	float D = geometricDistribution(alpha, ndh);
	float V = smithVisibilityG2(alpha, ndl, ndv);

	return D*V;
}

float directionalSpecBRDF(vec3 normal, vec3 eye, vec3 light, float roughness)
{
	vec3 h = normalize(eye+light);
	float ndh = max(0.0,dot(normal,h));
	float hdv = max(0.0,dot(h,eye));
	float hdl = max(0.0,dot(h,light));
	
	float f0 = 0.04; // Dielectric
	float f = f0+(1-f0)*pow(1-hdv,5.0);
	return f * specBRDF(roughness, ndh, hdv, hdl);
}