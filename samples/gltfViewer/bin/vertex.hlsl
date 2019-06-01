struct VertexPosColor
{
	float3 Position : position;
};

struct InstanceData
{
	matrix Model;
};
struct GlobalData
{
	matrix ViewProjection;
};

ConstantBuffer<InstanceData> gInstanceData : register(b0, space0);
ConstantBuffer<GlobalData> gGlobalData : register(b4, space0);

struct VertexShaderOutput
{
	float4 Color    : COLOR;
	float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
	VertexShaderOutput OUT;

	OUT.Position = mul(float4(IN.Position, 1.0f), gInstanceData.Model);
	//OUT.Position = float4(IN.Position, 1.0f);
	OUT.Color = float4(IN.Position*0.5+0.5, 1.0f);

	return OUT;
}