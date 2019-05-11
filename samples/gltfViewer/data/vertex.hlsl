struct VertexPosColor
{
	float3 Position : position;
};

/*struct ModelViewProjection
{
	matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);*/

struct VertexShaderOutput
{
	float4 Color    : COLOR;
	float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
	VertexShaderOutput OUT;

	//OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
	OUT.Position = float4(IN.Position, 1.0f);
	OUT.Color = float4(IN.Position*0.5+0.5, 1.0f);

	return OUT;
}