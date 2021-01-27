#include "../../Utility/UtilityShaderStruct.h"
#include "../../Material/MaterialBase.h"
#include "../../Utility/SettingData.h"

#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,   space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(CBV(b0,numDescriptors = 1,           space = 0, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = 2,           space = 1, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t2,numDescriptors = 1,           space = 1, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,   space = 2, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,   space = 3, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,   space = 4, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,   space = 5, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(SRV(t0,numDescriptors = unbounded,   space = 6, flags = DESCRIPTORS_VOLATILE)),"\
                          "DescriptorTable(CBV(b1,numDescriptors = 1,           space = 0, flags = DESCRIPTORS_VOLATILE)),"\
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

// 7,頂点情報配列
// 8,Pixel情報配列

// 9,設定

struct Output
{
	float4 svpos    : SV_POSITION;
	float4 pos		: POSITION;
	float2 uv       : TEXCOORD;
	uint instanceID : SV_InstanceID;
};

struct VertInf
{
	matrix posTrans;
	matrix uvTrans;
};

struct PixcelInf
{
	uint texIndex;
	float3 bright;
	float alpha;
};

Texture2D tex[512] : register(t0, space0);
// カメラ
ConstantBuffer<SceneStruct> scene : register(b0, space0);
Texture2D<float> depthTexVec[2]      : register(t0, space1);
StructuredBuffer<Utility> utility : register(t2, space1);

StructuredBuffer<MaterialBase> materialBase : register(t0, space2);
StructuredBuffer<int> addTexIndex : register(t0, space3);
StructuredBuffer<float> constandFloat : register(t0, space4);

StructuredBuffer<VertInf> vertInf : register(t0, space5);
StructuredBuffer<PixcelInf> pixcelInf : register(t0, space6);

// 設定
ConstantBuffer<SettingData> settingData : register(b1, space0);

SamplerState smp : register(s0);
SamplerComparisonState shadowSmp : register(s1);