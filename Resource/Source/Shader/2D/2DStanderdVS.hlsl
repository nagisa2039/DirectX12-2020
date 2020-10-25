#include "2DStanderd.hlsli"

struct VertInf
{
	matrix posTrans;
	matrix uvTrans;
};

StructuredBuffer<VertInf> vertInf : register(t0, space1);

[RootSignature(RS)]
Output VS(float4 pos : POSITION, float4 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
	Output o;
    o.svpos			= mul(vertInf[instanceID].posTrans, pos);
    o.uv			= mul(vertInf[instanceID].uvTrans, uv);
	o.instanceID	= instanceID;
	return o;
}