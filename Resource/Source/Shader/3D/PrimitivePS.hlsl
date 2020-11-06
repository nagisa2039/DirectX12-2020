#include "Primitive.hlsli"

// �e�N�X�`��
Texture2D<float4> tex[512] : register(t0, space1);

// �V���h�E�}�b�v�p	�f�v�X
Texture2D<float> lightDepthTex : register(t0);

SamplerState smp : register(s0);
SamplerComparisonState shadowSmp : register(s1);

struct PixelOutPut
{
	float4 col : SV_TARGET; //�J���[�l���o��
	//float4 normal : SV_TARGET1; //�@�����o��
	//float4 bright : SV_TARGET2; // �P�x�o��
};

[RootSignature(RS)]
PixelOutPut PS(Out input)
{
	PixelOutPut po;

	po.col = float4(1, 1, 1, 1);
	//return po;
	
	
	//float4 texColor = tmpTex.Sample(smp, input.uv);
	float3 light = normalize(float3(1, -1, 1));
	float3 rLight = reflect(light, input.normal.xyz);
	float3 eyeRay = normalize(input.pos.xyz - eye);
	float specB = saturate(dot(rLight, -eyeRay));
	if (specB)
	{
		specB = pow(specB, 20);
	}

	//float shadowWeight = 1.0f;
	//float3 posFromLight = input.tpos.xyz / input.tpos.w;
	//float2 shadowUV = (posFromLight.xy + float2(1, -1)) * float2(0.5f, -0.5f);
	//float shadowZ = lightDepthTex.Sample(smp, shadowUV);
	//if (posFromLight.z > shadowZ + 0.0005f)
	//{
	//	shadowWeight = 0.7f;
	//}

	float4 diffuse = float4(1, 1, 1, 1);
	float bright = saturate(dot(-light, input.normal.rgb));
	float4 difColor = saturate(float4(diffuse.rbg * bright, 1));
	float4 specColor = saturate(float4(specB, specB, specB, 0));
	float3 ambient = float3(0.2, 0.2, 0.2);

	float4 ret = float4(saturate((difColor.rgb/* * texColor.rgb*/) + specColor.rgb + ambient)/* * shadowWeight*/, diffuse.a);
	
	po.col = ret;
	//po.col = float4(shadowZ+0.1, shadowZ+0.1, shadowZ+0.1, 1);
	po.col.a = 1;

	//po.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	//po.normal.a = input.normal.a = 1;

	//po.bright = float4(0, 0, 0, 1);
	//float b = dot(po.col.rgb, float3(0.3f, 0.6, 0.1f)) > 0.9 ? 1.0f : 0.0f;
	//{
	//	po.bright = float4(b, b, b, 1);
	//}

	return po;
}

float4 ShadowPS(float4 pos : SV_POSITION) : SV_TARGET
{
	return float4(1, 1, 1, 1);
}