#include "2DStanderd.hlsli"

[RootSignature(RS)]
Output VS(float4 pos : POSITION, float4 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
	Output o;
    o.svpos			= mul(vertInf[instanceID].posTrans, pos);
    o.uv			= mul(vertInf[instanceID].uvTrans, uv).xy;
	o.instanceID	= instanceID;
	return o;
}