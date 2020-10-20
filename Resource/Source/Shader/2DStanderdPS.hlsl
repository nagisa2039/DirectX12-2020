struct Input
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

float4 PS(Input input) : SV_TARGET
{
	return tex.Sample(smp, input.uv);
}