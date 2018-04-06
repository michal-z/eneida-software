#define ImguiRs \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
	"CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
	"DescriptorTable(SRV(t0), visibility = SHADER_VISIBILITY_PIXEL), " \
	"StaticSampler(s0, filter = FILTER_MIN_MAG_MIP_LINEAR, visibility = SHADER_VISIBILITY_PIXEL)"

struct VertexData
{
	float2 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : COLOR;
};

struct PixelData
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : COLOR;
};

#if defined(VS_IMGUI)

struct Transform
{
	float4x4 mat;
};
ConstantBuffer<Transform> s_Cb : register(b0);

[RootSignature(ImguiRs)]
PixelData ImguiVs(VertexData input)
{
	PixelData output;
	output.position = mul(s_Cb.mat, float4(input.position, 0.0f, 1.0f));
	output.texcoord = input.texcoord;
	output.color = input.color;
	return output;
}

#elif defined(PS_IMGUI)

Texture2D s_Texture : register(t0);
SamplerState s_Sampler : register(s0);

[RootSignature(ImguiRs)]
float4 ImguiPs(PixelData input) : SV_TARGET
{
	return input.color * s_Texture.Sample(s_Sampler, input.texcoord);
}

#endif