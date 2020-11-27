#pragma once
#include "StaticMesh.h"

class PlaneMesh :
	public StaticMesh
{
public:
	// (dx12�Ǘ��N���X, ���W, ��, ���s, �e�N�X�`���p�X)
	PlaneMesh(std::weak_ptr<Actor>owner, Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos,
		const float width, const float depth, std::wstring texPath = L"");
	~PlaneMesh(); 

private:
	float width_;
	float depth_;
};