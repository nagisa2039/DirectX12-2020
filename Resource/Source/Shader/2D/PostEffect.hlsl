#include "2DStanderd.hlsli"

[RootSignature(RS)]
float4 PS(Output input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}