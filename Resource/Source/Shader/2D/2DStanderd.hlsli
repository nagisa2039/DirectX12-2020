#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
                          "DescriptorTable(SRV(t0,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(CBV(b0,numDescriptors = 1,space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                          "StaticSampler(s0 ,"\
                                             "filter = FILTER_MIN_MAG_MIP_LINEAR,"\
                                             "addressU = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressV = TEXTURE_ADDRESS_CLAMP,"\
                                             "addressW = TEXTURE_ADDRESS_CLAMP)"

struct Output
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};