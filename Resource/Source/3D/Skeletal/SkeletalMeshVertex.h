#include "../../Utility/ComvertCplusHLSL.h"

/// <summary>
/// スケルタルメッシュ頂点
/// </summary>
struct SkeletalMeshVertex
{
	// 座標
	float3 pos;
	// 法線
	float3 normal;
	// UV
	float2 uv;
	// ボーン番号
	int4 boneIdx;
	// ボーン影響度
	float4 weight;
};