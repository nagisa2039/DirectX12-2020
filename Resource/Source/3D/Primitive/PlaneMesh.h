#pragma once
#include "PrimitiveMesh.h"

class PlaneMesh :
	public PrimitiveMesh
{
public:
	// (dx12管理クラス, 座標, 幅, 奥行, テクスチャパス)
	PlaneMesh(Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, 
		const float width, const float depth, std::wstring texPath = L"");
	~PlaneMesh();

private:
	float width_;
	float depth_;
};

