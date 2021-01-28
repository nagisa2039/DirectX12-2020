#include "../../Utility/ComvertCplusHLSL.h"

/// <summary>
/// �X�P���^�����b�V�����_
/// </summary>
struct SkeletalMeshVertex
{
	// ���W
	float3 pos;
	// �@��
	float3 normal;
	// UV
	float2 uv;
	// �{�[���ԍ�
	int4 boneIdx;
	// �{�[���e���x
	float4 weight;
};