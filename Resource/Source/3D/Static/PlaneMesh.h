#pragma once
#include "StaticMesh.h"

/// <summary>
/// プレーン
/// </summary>
class PlaneMesh :
	public StaticMesh
{
public:
	/// <param name="owner">所有Actor</param>
	/// <param name="dx12">dx12管理クラス</param>
	/// <param name="pos">座標</param>
	/// <param name="width">幅</param>
	/// <param name="depth">奥行</param>
	/// <param name="texPath">テクスチャパス</param>
	PlaneMesh(std::weak_ptr<Actor>owner, Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos,
		const float width, const float depth, std::wstring texPath = L"");
	~PlaneMesh(); 
};