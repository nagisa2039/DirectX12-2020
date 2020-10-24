#include "2DStanderd.hlsli"

SamplerState smp : register(s0);
Texture2D<float4> tex[512] : register(t0, space0);

struct PixcelInf
{
	uint texIndex;
};

StructuredBuffer<PixcelInf> pixcelInf : register(t0, space2);

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
    float3 bright = float3(1.0f, 1.0f, 1.0f);
    float alpha = 255.0f/255.0f;
    
    float4 texColor = tex[pixcelInf[input.instanceID].texIndex].Sample(smp, input.uv);
    return float4(texColor.rgb * bright, texColor.a * alpha);
}