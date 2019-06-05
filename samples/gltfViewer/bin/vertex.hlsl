struct VertexPosColor
{
	float3 Position : position;
};

struct InstanceData
{
	matrix World;
	matrix WorldViewProjection;
};

ConstantBuffer<InstanceData> gInstanceData : register(b0, space0);

struct VertexShaderOutput
{
	float4 Color    : COLOR;
	float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
	VertexShaderOutput OUT;

	float4 vertex = float4(IN.Position, 1.0f);
	OUT.Position = mul(vertex, gInstanceData.WorldViewProjection);
	OUT.Color = float4(IN.Position*0.5+0.5, 1.0f);

	return OUT;
}