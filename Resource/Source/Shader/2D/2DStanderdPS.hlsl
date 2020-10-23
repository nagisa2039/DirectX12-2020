#include "2DStanderd.hlsli"

SamplerState smp : register(s0);
Texture2D<float4> tex[512] : register(t0);

struct PixcelInf
{
	uint texIndex;
};

cbuffer index : register(b0, space2)
{
	PixcelInf pixelInf[512];
}

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	//if (input.instanceID == 0)
	//{
	//	return float4(1, 0, 0, 1);
	//}
	
	//return float4(0, 0, 1, 1);
	
	return tex[pixelInf[input.instanceID].texIndex].Sample(smp, input.uv);
}