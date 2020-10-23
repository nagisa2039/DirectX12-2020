#include "2DStanderd.hlsli"

struct VertInf
{
	matrix posTrans;
	matrix uvTrans;
};

cbuffer trans : register(b0, space1)
{
	VertInf vertInf[512];
}

[RootSignature(RS)]
Output VS(float4 pos : POSITION, float4 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
	Output o;
	o.svpos = mul(vertInf[1].posTrans, pos);
	o.uv	= mul(vertInf[1].uvTrans,  uv);
	
	o.svpos = pos;
	o.uv = uv;
	o.instanceID = instanceID;
	return o;
}