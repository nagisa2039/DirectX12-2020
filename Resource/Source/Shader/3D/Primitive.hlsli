#include "../../Utility/UtilityShaderStruct.h"
#include "../../Material/MaterialBase.h"

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
						"DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(CBV(b0,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(SRV(t0,numDescriptors = 2,space = 1, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(SRV(t2,numDescriptors = 1,space = 1, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 2, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 3, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 4, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(CBV(b1,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
						"StaticSampler(s0 ,"\
                                            "filter = FILTER_MIN_MAG_MIP_LINEAR,"\
                                            "addressU = TEXTURE_ADDRESS_CLAMP,"\
                                            "addressV = TEXTURE_ADDRESS_CLAMP,"\
                                            "addressW = TEXTURE_ADDRESS_CLAMP),"\
						"StaticSampler(s1 ,"\
											"filter = FILTER_MIN_MAG_MIP_LINEAR,"\
											"addressU = TEXTURE_ADDRESS_CLAMP,"\
											"addressV = TEXTURE_ADDRESS_CLAMP,"\
											"addressW = TEXTURE_ADDRESS_CLAMP,"\
											"comparisonFunc = COMPARISON_LESS_EQUAL)"

// 0,�e�N�X�`���z��
// 1,�J����
// 2,�[�x
// 3,utility�萔

// 4,�}�e���A���x�[�X
// 5,�ǉ��e�N�X�`���C���f�b�N�X
// 6,�ǉ��萔(float)

// 7,���W

Texture2D tex[512] : register(t0, space0);
cbuffer cameraBuffer : register(b0, space0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // ���_
};
Texture2D<float> depthTex[2] : register(t0, space1);
StructuredBuffer<Utility> utility : register(t2, space1);

StructuredBuffer<MaterialBase> materialBase : register(t0, space2);
StructuredBuffer<int> addTexIndex : register(t0, space3);
StructuredBuffer<float> constandFloat : register(t0, space4);

cbuffer worldBuffer : register(b1)
{
	matrix world;
};

SamplerState smp : register(s0);
SamplerComparisonState shadowSmp : register(s1);

struct VertexOut
{
	float4 svpos : SV_POSITION; // Pipeline�ɓ����邽�߂ɂ�SV_POSITION���K�v	�J��������̍��W
	float4 pos : POSITION; // ���[���h���W
	float4 tpos : POSITION1;
	float4 normal : NORMAL; // �@�����
	float2 uv : TEXCOORD; // UV���
};

// Pixel�o��
struct PixelOut
{
	float4 color : SV_Target0; //�J���[�l���o��
	float4 normal : SV_Target1; //�@�����o��
	float4 bright : SV_Target2; // �P�x�o��
};