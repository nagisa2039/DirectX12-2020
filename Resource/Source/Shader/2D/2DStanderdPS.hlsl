#include "2DStanderd.hlsli"

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	return tex.Sample(smp, input.uv);
}