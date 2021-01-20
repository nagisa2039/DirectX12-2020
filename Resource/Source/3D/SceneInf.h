#pragma once
#include "Utility/ComPtr.h"
#include <memory>

class Command;
class ID3D12Device;
class ID3D12Resource;
class ID3D12DescriptorHeap;
class CameraObject;
struct SceneStruct;

/// <summary>
/// 3D�V�[�������Ǘ�
/// </summary>
class SceneInf
{
public:
	/// <param name="cmd">�R�}���h�Ǘ��N���X</param>
	/// <param name="dev">�f�o�C�X</param>
	SceneInf(Command& cmd, ID3D12Device& dev);
	~SceneInf();

	/// <summary>
	/// ������
	/// </summary>
	void Init();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �J�����̃q�[�v��ݒ�
	/// </summary>
	/// <param name="rootParamIdx">rootSignature�C���f�b�N�X</param>
	void SetCameraDescriptorHeap(const unsigned int rootParamIdx);

	/// <summary>
	/// ImGui�̕`��
	/// </summary>
	void DrawImGui();

	/// <summary>
	/// �J�����I�u�W�F�N�g�̎擾
	/// </summary>
	std::shared_ptr<CameraObject>& GetCameraObject();

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

	// �J����
	std::shared_ptr<CameraObject> cameraObj_;
	// ���C�g
	std::shared_ptr<CameraObject> lightObj_;

	// �J�����̍쐬
	bool CreateCameraConstantBufferAndView();
};

