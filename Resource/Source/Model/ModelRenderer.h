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

	void DrawToMyScreen();

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

	ComPtr<ID3DBlob> vertexShader_ = nullptr;
	ComPtr<ID3DBlob> pixelShader_ = nullptr;

	bool CreateModelRS();
	void CreateRSRootParameter(std::vector<D3D12_ROOT_PARAMETER>& rps, std::vector<D3D12_DESCRIPTOR_RANGE>& ranges);
	void CreateRSDescriptorRange(std::vector<D3D12_DESCRIPTOR_RANGE>& ranges);
	void CreateRSSampler(std::vector<D3D12_STATIC_SAMPLER_DESC>&  samplers);
	bool CreateModelPL();
};

