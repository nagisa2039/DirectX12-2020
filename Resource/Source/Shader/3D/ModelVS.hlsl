#include "Model.hlsli"

matrix GetTransform(const int4 boneno, const float4 weight)
{
	matrix a;
	matrix b;
	matrix v = a + b * boneMats[boneno.x];
	
	matrix mixMat = 
		  boneMats[boneno.x] * weight.x
		+ boneMats[boneno.y] * weight.y
		+ boneMats[boneno.z] * weight.z
		+ boneMats[boneno.w] * weight.w;
	
	return mixMat = mul(world, mixMat);
}

//頂点シェーダ
[RootSignature(RS)]
VertexOut VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD,
	int4 boneno : BONENO, float4 weight : WEIGHT, uint instanceID : SV_InstanceID)
{
	VertexOut o;
	
	matrix transform = GetTransform(boneno, weight);

	pos.z += 5 * instanceID;
	pos = mul(transform, pos);

	matrix camera = mul(scene.proj, scene.view);
	o.svpos = mul(camera, pos);

	o.pos = pos;

	o.posFromLight = mul(scene.lightCamera, pos);

	normal.w = 0;
	o.normal = mul(transform, normal);

	o.uv = uv;

	return o;
}

// 影用座標変換
[RootSignature(RS)]
ShadowVertexOut ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD,
	int4 boneno : BONENO, float4 weight : WEIGHT)
{
	ShadowVertexOut vo;
	matrix transform = GetTransform(boneno, weight);
	vo.svpos = mul(scene.lightCamera, mul(transform, pos));
	vo.uv = uv;
	
	return vo;
}