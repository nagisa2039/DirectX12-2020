#pragma once
#include "StaticMesh.h"

/// <summary>
/// �v���[��
/// </summary>
class PlaneMesh :
	public StaticMesh
{
public:
	/// <param name="owner">���LActor</param>
	/// <param name="dx12">dx12�Ǘ��N���X</param>
	/// <param name="pos">���W</param>
	/// <param name="width">��</param>
	/// <param name="depth">���s</param>
	/// <param name="texPath">�e�N�X�`���p�X</param>
	PlaneMesh(std::weak_ptr<Actor>owner, Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos,
		const float width, const float depth, std::wstring texPath = L"");
	~PlaneMesh(); 
};