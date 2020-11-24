#pragma once
#include "Utility/ComPtr.h"
#include <DirectXMath.h>
#include <d3d12.h>

// �g�����X�t�H�[�����
struct Transform
{
	// ���W
	DirectX::XMFLOAT3 pos		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// ��]
	DirectX::XMFLOAT3 rotate	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// �g��
	DirectX::XMFLOAT3 scale		= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
};

/// <summary>
/// 3D�I�u�W�F�N�g���N���X
/// </summary>
class Actor
{
public:
	Actor();
	~Actor();

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// �g�����X�t�H�[���̎擾
	/// </summary>
	const Transform& GetTransform()const;

	/// <summary>
	/// �g�����X�t�H�[���̐ݒ�
	/// </summary>
	void SetTransform(const Transform& transform);

	/// <summary>
	/// Transform�̍X�V
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// CommandList��trans����ǉ�(�Z�b�g)����
	/// </summary>
	/// <param name="rootParamatorIndex">rootParamator�̃C���f�b�N�X</param>
	void SetTransformHeap(const UINT rootParamatorIndex);

protected:
	/// <summary>
	/// �g�����X�t�H�[�������i�[�����q�[�v�̎擾
	/// </summary>
	const ComPtr<ID3D12DescriptorHeap>& GetTransformHeap()const;

private:
	Transform trans_;
	DirectX::XMMATRIX* mappedTrans_;

	// ���W�s��p�萔�o�b�t�@
	ComPtr<ID3D12Resource> transCB_ = nullptr;
	// transCB������q�[�v
	ComPtr<ID3D12DescriptorHeap> transHeap_ = nullptr;
};

