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

// 0,テクスチャ配列
// 1,カメラ
// 2,深度
// 3,utility定数

// 4,マテリアルベース
// 5,追加テクスチャインデックス
// 6,追加定数(float)

// 7,座標

Texture2D tex[512] : register(t0, space0);
cbuffer cameraBuffer : register(b0, space0)
{
	matrix view;
	matrix proj;
	matrix invProj;
	matrix lightCamera;
	matrix shadow;
	float3 eye; // 視点
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
	float4 svpos : SV_POSITION; // Pipelineに投げるためにはSV_POSITIONが必要	カメラからの座標
	float4 pos : POSITION; // ワールド座標
	float4 tpos : POSITION1;
	float4 normal : NORMAL; // 法線情報
	float2 uv : TEXCOORD; // UV情報
};

// Pixel出力
struct PixelOut
{
	float4 color : SV_Target0; //カラー値を出力
	float4 normal : SV_Target1; //法線を出力
	float4 bright : SV_Target2; // 輝度出力
};