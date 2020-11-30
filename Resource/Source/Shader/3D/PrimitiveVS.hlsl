#include "Primitive.hlsli"

[RootSignature(RS)]
VertexOut VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	VertexOut o;
	pos = mul(world, pos);
	matrix camera = mul(scene.proj, scene.view);
	o.svpos = mul(camera, pos);
	o.tpos = mul(scene.lightCamera, pos);
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
	return mul(scene.lightCamera, mul(world, pos));
}