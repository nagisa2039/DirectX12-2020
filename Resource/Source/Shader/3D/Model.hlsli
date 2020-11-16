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

// 0 テクスチャ配列
// 1 マテリアル配列
// 2 カメラ
// 3 座標
// 4 設定
// 5 マテリアルインデックス配列
// 6 深度テクスチャ

// 頂点出力
struct VertexOut
{
	float4 svpos : SV_POSITION; // Pipelineに投げるためにはSV_POSITIONが必要	カメラからの座標
	float4 pos : POSITION; // ワールド座標
	float4 tpos : POSITION1; // 頂点変換後の座標
	float4 normal : NORMAL; // 法線情報
	float2 uv : TEXCOORD; // UV情報
};

// 座標変換用スロット
cbuffer transBuffer : register(b0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // 視点
};

// Pixel出力
struct PixelOut
{
	float4 color	: SV_Target0; //カラー値を出力
	float4 normal	: SV_Target1; //法線を出力
	float4 bright	: SV_Target2; // 輝度出力
};