#include "2DStanderd.hlsli"

SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);

cbuffer trans : register(b0)
{
	matrix posTrans;
	matrix uvTrans;
}

//頂点シェーダ
Output VS(float4 pos : POSITION, float4 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
	Output o;

	o.svpos = mul(posTrans, pos);

	float4 calUV = mul(uvTrans, uv);
	o.uv = float2(calUV.x, calUV.y);

	return o;
}

//ピクセルシェーダ
float4 PS(Output input) : SV_Target
{
	return tex.Sample(smp, input.uv);
}