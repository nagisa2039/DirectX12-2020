#pragma once
#include "Utility/ComPtr.h"
#include <d3d12.h>
#include <DirectXMath.h>

struct SceneStruct;
class Command;

class Camera
{
public:
	Camera(Command& cmd, ID3D12Device& dev);
	~Camera();

	void Update();

	/// <summary>
	/// �J�����̃q�[�v��ݒ�
	/// </summary>
	/// <param name="rootParamIdx">rootSignature�C���f�b�N�X</param>
	void SetCameraDescriptorHeap(const UINT rootParamIdx);

	/// <summary>
	/// �J�����̍��W�擾
	/// </summary>
	DirectX::XMFLOAT3 GetCameraPosition()const;

	/// <summary>
	/// �J�����̃^�[�Q�b�g���W�̎擾
	/// </summary>
	DirectX::XMFLOAT3 GetCameraTarget()const;

	/// <summary>
	/// ���C�g�̌������擾
	/// </summary>
	DirectX::XMFLOAT3 GetLightVec()const;

	/// <summary>
	/// �J�����̍��W�ݒ�
	/// </summary>
	/// <param name="pos">�J�������W</param>
	void SetCameraPosision(const DirectX::XMFLOAT3& pos);

	/// <summary>
	/// �J�����̃^�[�Q�b�g���W�ݒ�
	/// </summary>
	/// <param name="target">�^�[�Q�b�g���W</param>
	void SetCameraTarget(const DirectX::XMFLOAT3& target);

	/// <summary>
	/// �J�����s��̍X�V
	/// </summary>
	void UpdateCamera();

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

	DirectX::XMFLOAT3 eye_;
	DirectX::XMFLOAT3 target_;
	DirectX::XMFLOAT3 up_;
	float fov_;

	// �J�����̍쐬
	bool CreateCameraConstantBufferAndView();
};

