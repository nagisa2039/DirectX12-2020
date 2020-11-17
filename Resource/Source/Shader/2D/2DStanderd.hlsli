#include "../../Utility/UtilityShaderStruct.h"
#include "../../2D/MaterialBase.h"

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 1, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 2, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = 2,space = 3, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = 1,space = 4, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = 1,space = 5, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 6, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 7, flags = DESCRIPTORS_VOLATILE)),"\
                          "StaticSampler(s0 ,"\
                                             "filter = FILTER_MIN_MAG_MIP_LINEAR,"\
                                             "addressU = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressV = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressW = TEXTURE_ADDRESS_CLAMP)"

// 0,�e�N�X�`���z��
// 1,���_���z��
// 2,Pixel���z��
// 3,�[�x
// 4,utility�萔
// 5,�ǉ��e�N�X�`���C���f�b�N�X
// 6,�ǉ��萔(float)

struct Output
{
	float4 svpos    : SV_POSITION;
	float4 pos		: POSITION;
	float2 uv       : TEXCOORD;
	uint instanceID : SV_InstanceID;
};

SamplerState smp : register(s0);
Texture2D tex[512] : register(t0, space0);

struct VertInf
{
	matrix posTrans;
	matrix uvTrans;
};

StructuredBuffer<VertInf> vertInf : register(t0, space1);

struct PixcelInf
{
	uint texIndex;
	float3 bright;
	float alpha;
};

StructuredBuffer<PixcelInf> pixcelInf : register(t0, space2);

Texture2D<float> depthTex[2]      : register(t0, space3);

StructuredBuffer<Utility> utility : register(t0, space4);

StructuredBuffer<MaterialBase> materialBase : register(t0, space5);

StructuredBuffer<int> addTexIndex : register(t0, space6);

StructuredBuffer<float> constandFloat : register(t0, space7);