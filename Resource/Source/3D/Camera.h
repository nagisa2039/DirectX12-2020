#pragma once
#include "Utility/ComPtr.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "Utility/Geometry.h"

class Command;
class ID3D12Device;

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
	Vector3 GetCameraPosition()const;

	/// <summary>
	/// �J�����̃^�[�Q�b�g���W�̎擾
	/// </summary>
	Vector3 GetCameraTarget()const;

	/// <summary>
	/// �J�����̍��W�ݒ�
	/// </summary>
	/// <param name="pos">�J�������W</param>
	void SetCameraPosision(const Vector3& pos);

	/// <summary>
	/// �J�����̃^�[�Q�b�g���W�ݒ�
	/// </summary>
	/// <param name="target">�^�[�Q�b�g���W</param>
	void SetCameraTarget(const Vector3& target);

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

	// GPU�ɓn���s����܂Ƃ߂��\����
	struct Scene
	{
		//DirectX::XMMATRIX world;	// �����O���[���h
		DirectX::XMMATRIX view;	//
		DirectX::XMMATRIX proj;	//
		DirectX::XMMATRIX invProj;	// �v���W�F�N�V�����̋t�s��
		DirectX::XMMATRIX lightCamera;	//���C�h���猩���r���[�v���W�F�N�V����
		DirectX::XMMATRIX shadow;
		DirectX::XMFLOAT3 eye;	// ���_
	};

	// �萔�o�b�t�@�̃A�h���X���i�[	
	// _cameraCB�̓��e��ύX�������Ƃ��͂�����ʂ��ĕύX���Ă�
	Scene* mappedCam_;

	DirectX::XMFLOAT3 eye_;
	DirectX::XMFLOAT3 target_;
	DirectX::XMFLOAT3 up_;
	float fov_;

	// �J�����̍쐬
	bool CreateCameraConstantBufferAndView();
};
