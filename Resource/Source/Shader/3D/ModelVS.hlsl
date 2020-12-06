#include "Model.hlsli"

//頂点シェーダ
[RootSignature(RS)]
VertexOut VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD,
	int4 boneno : BONENO, float4 weight : WEIGHT, uint instanceID : SV_InstanceID)
{
    VertexOut o;
    pos.z += 5 * instanceID;
    matrix camera = mul(scene.proj, scene.view);
    o.svpos = mul(camera, pos);
    o.pos = pos;
    o.posFromLight = mul(scene.lightCamera, pos);
    o.normal = normal;
    o.uv = uv;
    return o;
}

// 影用座標変換
[RootSignature(RS)]
ShadowVertexOut ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD,
	int4 boneno : BONENO, float4 weight : WEIGHT)
{
	ShadowVertexOut vo;
    vo.svpos = mul(scene.lightCamera, pos);
	vo.uv = uv;
	
	return vo;
}