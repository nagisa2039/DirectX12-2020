#pragma once
#include "PrimitiveMesh.h"

class PlaneMesh :
	public PrimitiveMesh
{
private:

	float _width;
	float _depth;

public:
	// (dx12�Ǘ��N���X, ���W, ��, ���s, �e�N�X�`���p�X)
	PlaneMesh(Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, const float width, const float depth, std::wstring texPath = L"");

	~PlaneMesh();
};
