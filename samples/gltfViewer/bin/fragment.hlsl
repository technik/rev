struct PixelShaderInput
{
	float4 Color    : COLOR;
};

float4 main(PixelShaderInput IN) : SV_Target
{
	return IN.Color + float4(0.5, 0.5, 0.0, 0.0);
}