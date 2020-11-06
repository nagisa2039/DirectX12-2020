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

// 0 カメラ
// 1 座標
// 2 深度テクスチャ
// 3 テクスチャ

struct Out
{
	float4 svpos : SV_POSITION; // Pipelineに投げるためにはSV_POSITIONが必要	カメラからの座標
	float4 pos : POSITION; // ワールド座標
	float4 tpos : POSITION1;
	float4 normal : NORMAL; // 法線情報
	float2 uv : TEXCOORD; // UV情報
};

cbuffer transBuffer : register(b0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // 視点
};