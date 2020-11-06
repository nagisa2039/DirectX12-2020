#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
						"DescriptorTable(CBV(b0,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(CBV(b1,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                        "DescriptorTable(SRV(t0,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                        "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 1, flags = DESCRIPTORS_VOLATILE)),"\
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

// 0 �J����
// 1 ���W
// 2 �[�x�e�N�X�`��
// 3 �e�N�X�`��

struct Out
{
	float4 svpos : SV_POSITION; // Pipeline�ɓ����邽�߂ɂ�SV_POSITION���K�v	�J��������̍��W
	float4 pos : POSITION; // ���[���h���W
	float4 tpos : POSITION1;
	float4 normal : NORMAL; // �@�����
	float2 uv : TEXCOORD; // UV���
};

cbuffer transBuffer : register(b0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // ���_
};