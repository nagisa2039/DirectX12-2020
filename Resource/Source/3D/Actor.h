#pragma once
#include "Utility/ComPtr.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <memory>
#include <vector>
#include <string>

class Component;

// �g�����X�t�H�[�����
struct Transform
{
	// ���W
	DirectX::XMFLOAT3 pos		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// ��]
	DirectX::XMFLOAT3 rotate	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// �g��
	DirectX::XMFLOAT3 scale		= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	/// <summary>
	/// Transform�s��̎擾
	/// </summary>
	DirectX::XMMATRIX GetMatrix()const;

	/// <summary>
	/// ��]�s��̎擾
	/// </summary>
	DirectX::XMMATRIX GetRotateMatrix()const;

	/// <summary>
	/// �O�����x�N�g���̎擾
	/// </summary>
	DirectX::XMVECTOR GetForwerd()const;

	/// <summary>
	/// ������x�N�g���̎擾
	/// </summary>
	DirectX::XMVECTOR GetUp()const;

	/// <summary>
	/// �E�����x�N�g���̎擾
	/// </summary>
	DirectX::XMVECTOR GetRight()const;
};

/// <summary>
/// 3D�I�u�W�F�N�g���N���X
/// </summary>
class Actor
	: public std::enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update();

	/// <summary>
	/// ���O�̎擾
	/// </summary>
	const std::string& GetName()const;

	/// <summary>
	/// ���O�̐ݒ�
	/// </summary>
	/// <param name="n">�ݒ肷�閼�O</param>
	void SetName(const std::string& n);

	/// <summary>
	/// �g�����X�t�H�[���̎擾
	/// </summary>
	Transform& GetTransform();

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
	/// <param name="compute">Compute�p��</param>
	void SetTransformHeap(const UINT rootParamatorIndex, const bool compute = false);

	/// <summary>
	/// �R���|�[�l���g�̒ǉ�
	/// </summary>
	/// <param name="component">�Ώۂ̃R���|�[�l���g</param>
	void AddComponent(std::shared_ptr<Component> component);

	/// <summary>
	/// ImGui�̕`��
	/// </summary>
	/// <param name="num">Node���ʔԍ�</param>
	virtual void DrawImGui(const int num);

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

	std::vector<std::shared_ptr<Component>> components_;

	std::string name_;
};

