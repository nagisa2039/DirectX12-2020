#include "Model.hlsli"

// ���W�ϊ��p�X���b�g
cbuffer transBuffer : register(b1)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // ���_
};

// ���W�s��p�X���b�g
cbuffer worldBuffer : register(b2)
{
	matrix world;
};

// �{�[���s��
cbuffer bones : register(b3)
{
	matrix boneMats[512];
};

//���_�V�F�[�_
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

	o.instanceID = instanceID;

	return o;
}