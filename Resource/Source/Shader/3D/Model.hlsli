#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
                        "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                        "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 1, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(CBV(b0,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
						"DescriptorTable(CBV(b1,numDescriptors = 2,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                        "DescriptorTable(CBV(b3,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                        "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 2, flags = DESCRIPTORS_VOLATILE)),"\
                        "DescriptorTable(SRV(t0,numDescriptors = 1,space = 3, flags = DESCRIPTORS_VOLATILE)),"\
                        "StaticSampler(s0 ,"\
                                            "filter = FILTER_MIN_MAG_MIP_LINEAR,"\
                                            "addressU = TEXTURE_ADDRESS_WRAP,"\
                                             "addressV = TEXTURE_ADDRESS_WRAP,"\
                                             "addressW = TEXTURE_ADDRESS_WRAP),"\
						  "StaticSampler(s1 ,"\
                                             "filter = FILTER_COMPARISON_MIN_MAG_MIP_POINT,"\
                                             "addressU = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressV = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressW = TEXTURE_ADDRESS_CLAMP),"\
						 "StaticSampler(s2 ,"\
                                             "filter = FILTER_MIN_MAG_MIP_LINEAR,"\
                                             "addressU = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressV = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressW = TEXTURE_ADDRESS_CLAMP,"\
											 "comparisonFunc = COMPARISON_LESS_EQUAL)"

// 0 �e�N�X�`���z��
// 1 �}�e���A���z��
// 2 �J����
// 3 ���W
// 4 �ݒ�
// 5 �}�e���A���C���f�b�N�X�z��
// 6 �[�x�e�N�X�`��

// ���_�o��
struct VertexOut
{
	float4 svpos : SV_POSITION; // Pipeline�ɓ����邽�߂ɂ�SV_POSITION���K�v	�J��������̍��W
	float4 pos : POSITION; // ���[���h���W
	float4 tpos : POSITION1; // ���_�ϊ���̍��W
	float4 normal : NORMAL; // �@�����
	float2 uv : TEXCOORD; // UV���
};

// ���W�ϊ��p�X���b�g
cbuffer transBuffer : register(b0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // ���_
};

// Pixel�o��
struct PixelOut
{
	float4 color	: SV_Target0; //�J���[�l���o��
	float4 normal	: SV_Target1; //�@�����o��
	float4 bright	: SV_Target2; // �P�x�o��
};