#include "Primitive.hlsli"

cbuffer worldBuffer : register(b1)
{
	matrix world;
};

[RootSignature(RS)]
VertexOut VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	VertexOut o;
	pos = mul(world, pos);
	matrix camera = mul(proj, view);
	o.svpos = mul(camera, pos);
	o.tpos = mul(lightCamera, pos);
	o.pos = pos;

	normal.w = 0;
	o.normal = mul(world, normal);

	o.uv = uv;

	return o;
}

[RootSignature(RS)]
float4 ShadowVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
:SV_Position
{
	return mul(lightCamera, mul(world, pos));
}