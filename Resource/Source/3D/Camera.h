#pragma once
#include "Utility/ComPtr.h"
#include "3D/Component.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "3D/Actor.h"

struct SceneStruct;
class Command;

/// <summary>
/// �J�����N���X
/// </summary>
class Camera
	: public Component
{
public:
	Camera(Command& cmd, ID3D12Device& dev, std::weak_ptr<Actor>owner);
	~Camera();

	/// <summary>
	/// ������
	/// </summary>
	void Init()override;

	/// <summary>
	/// �X�V
	/// </summary>
	void Update()override;

	/// <summary>
	/// �J�����̃q�[�v��ݒ�
	/// </summary>
	/// <param name="rootParamIdx">rootSignature�C���f�b�N�X</param>
	void SetCameraDescriptorHeap(const UINT rootParamIdx);

	/// <summary>
	/// �^�[�Q�b�g���W�̎擾
	/// </summary>
	DirectX::XMFLOAT3 GetTargetPos()const;

	/// <summary>
	/// �^�[�Q�b�g���W�̐ݒ�
	/// </summary>
	/// <param name="tpos">�^�[�Q�b�g���W</param>
	void SetTargetPos(const DirectX::XMFLOAT3& tpos);

	/// <summary>
	/// �^�[�Q�b�g�ւ̃x�N�g�����擾����
	/// </summary>
	DirectX::XMFLOAT3 GetTargetVec()const;

	/// <summary>
	/// View�s��̎擾
	/// </summary>
	DirectX::XMMATRIX GetViewMatrix()const;

	/// <summary>
	/// Projection�s��̎擾
	/// </summary>
	DirectX::XMMATRIX GetProjMatrix()const;

	/// <summary>
	/// ImGui�̕`��
	/// </summary>
	void DrawImGui()override;

private:
	Command& cmd_;
	ID3D12Device& dev_;

	// �J�����s��p�萔�o�b�t�@
	ComPtr<ID3D12Resource> cameraCB_ = nullptr;
	// transCB������q�[�v
	ComPtr<ID3D12DescriptorHeap> cameraHeap_ = nullptr;

	// �萔�o�b�t�@�̃A�h���X���i�[	
	// _cameraCB�̓��e��ύX�������Ƃ��͂�����ʂ��ĕύX���Ă�
	SceneStruct* mappedScene_;

	DirectX::XMFLOAT3 target_;
	float fov_;

	// �J�����̍쐬
	bool CreateCameraConstantBufferAndView();

	/// <summary>
	/// �J�����s��̍X�V
	/// </summary>
	void UpdateCamera();
};

/// <summary>
/// �J�����I�u�W�F�N�g
/// </summary>
class CameraObject
	: public Actor
{
public:
	/// <param name="cmd"></param>
	/// <param name="dev"></param>
	CameraObject(Command& cmd, ID3D12Device& dev);
	~CameraObject();

	void Update()override;

	std::shared_ptr<Camera>& GetCamera();

private:
	Command& cmd_;
	ID3D12Device& dev_;
	std::shared_ptr<Camera> camera_;
};