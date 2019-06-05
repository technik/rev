struct VertexInput
{
	float3 Position : position;
	float3 Normal : normal;
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

VertexShaderOutput main(VertexInput IN)
{
	VertexShaderOutput OUT;

	float4 vertex = float4(IN.Position, 1.0f);
	OUT.Position = mul(vertex, gInstanceData.WorldViewProjection);
	float4 localNormal = float4(IN.Normal*0.5+0.5, 0.0f);
	OUT.Color = mul(gInstanceData.World, localNormal);
	OUT.Color.w = 1.0f;
	return OUT;
}