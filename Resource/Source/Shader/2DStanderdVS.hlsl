#include "2DStanderd.hlsli"

cbuffer trans : register(b0)
{
	matrix posTrans;
	matrix uvTrans;
}

[RootSignature(RS)]
Output VS(float4 pos : POSITION, float4 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
	Output o;
	o.svpos = mul(posTrans, pos);
	float4 calUV = mul(uvTrans, uv);
	o.uv = float2(calUV.x, calUV.y);
	return o;
}