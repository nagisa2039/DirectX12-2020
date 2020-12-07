#include "../../Utility/ComvertCplusHLSL.h"

struct SkeletalMeshVertex
{
	float3 pos;
	float3 normal;
	float2 uv;
	int4 boneIdx;
	float4 weight;
};