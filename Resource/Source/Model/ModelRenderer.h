#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <map>
#include <string>

using Microsoft::WRL::ComPtr;
class ModelActor;
class Dx12Wrapper;
class VMDMotion;


class ModelRenderer
{
public:
	ModelRenderer(Dx12Wrapper& dx12);
	~ModelRenderer();

	// ������
	bool Init();
	// �X�V
	void Update();
	// �`��
	void Draw();

	void DrawTo3DSpace();

	// RootSignature�̐ݒ�
	void SetModelRS();
	// PipelineState�̐ݒ�
	void SetModelPL();
	// �e�̕`��
	void DrawFramShadow();
	// ���[�V�������̎擾(���[�V�����t�@�C���p�X)
	VMDMotion& GetVMDMotion(std::string motionPath);

private:
	Dx12Wrapper& dx12_;

	ComPtr<ID3D12RootSignature> modelRS_ = nullptr;
	ComPtr<ID3D12PipelineState> modelPL_ = nullptr;

	// �e�p�p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> shadowPL_ = nullptr;

	std::vector<std::shared_ptr<ModelActor>> modelActors_;
	std::map<std::string, std::shared_ptr<VMDMotion>> vmdMotions_;

	int screenH_;
	int lightScreenH_;

	bool CreateModelPL();
};

