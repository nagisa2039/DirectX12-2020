#include "Model.hlsli"

// 座標変換用スロット
cbuffer transBuffer : register(b0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // 視点
};

// 座標行列用スロット
cbuffer worldBuffer : register(b1)
{
	matrix world;
};

// ボーン行列
cbuffer bones : register(b2)
{
	matrix boneMats[512];
};

//頂点シェーダ
[RootSignature(RS)]
Out VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD,
	int4 boneno : BONENO, float4 weight : WEIGHT, uint instanceID : SV_InstanceID)
{
	Out o;
	
	matrix mixMat = boneMats[boneno.x] * weight.x
				  + boneMats[boneno.y] * weight.y
				  + boneMats[boneno.z] * weight.z
				  + boneMats[boneno.w] * weight.w;

	mixMat = mul(world, mixMat);

    pos = mul(mixMat, pos);
	pos.z += 5 * instanceID;

	matrix camera = mul(proj, view);
	o.svpos = mul(camera, pos);

	o.pos = pos;

	o.tpos = mul(lightCamera, pos);

	normal.w = 0;
	o.normal = mul(mixMat, normal);

	o.uv = uv;

	return o;
}

// 影用座標変換
[RootSignature(RS)]
float4 ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD,
	int4 boneno : BONENO, float4 weight : WEIGHT) : SV_POSITION
{
	matrix mixMat = boneMats[boneno.x] * weight.x
		+ boneMats[boneno.y] * weight.y
		+ boneMats[boneno.z] * weight.z
		+ boneMats[boneno.w] * weight.w;

	pos = mul(world, mul(mixMat, pos));

	return mul(lightCamera, pos);
}