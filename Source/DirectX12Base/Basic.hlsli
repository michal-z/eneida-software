#define BasicRs \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
	"CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)"

#if defined(VS_BASIC)

struct ConstData
{
	float4x4 worldToProj;
};
ConstantBuffer<ConstData> s_Cb;

[RootSignature(BasicRs)]
float4 BasicVs(float3 pos : POSITION) : SV_POSITION
{
	return mul(float4(pos, 1.0f), s_Cb.worldToProj);
}

#elif defined(PS_BASIC)

[RootSignature(BasicRs)]
float4 BasicPs() : SV_TARGET
{
	return float4(0.0f, 0.5f, 0.0f, 1.0f);
}

#endif