#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 1, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,space = 2, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = 2,space = 3, flags = DESCRIPTORS_VOLATILE)),"\
                          "StaticSampler(s0 ,"\
                                             "filter = FILTER_MIN_MAG_MIP_LINEAR,"\
                                             "addressU = TEXTURE_ADDRESS_WRAP,"\
                                             "addressV = TEXTURE_ADDRESS_WRAP,"\
                                             "addressW = TEXTURE_ADDRESS_WRAP)"

struct Output
{
	float4 svpos    : SV_POSITION;
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

Texture2D<float> depthTex      : register(t0, space3);
Texture2D<float> lightDepthTex : register(t1, space3);