#include "ModelRenderer.h"
#include <cassert>
#include "d3dx12.h"
#include "ModelActor.h"
#include "Utility/Tool.h"
#include "System/Dx12Wrapper.h"
#include "System/Command.h"
#include "VMDMotion.h"
#include "System/Application.h"
#include "System/TexLoader.h"
#include "2D/SpriteDrawer.h"
#include "Utility/Constant.h"
#include "Utility/dx12Tool.h"
#include "System/ShaderLoader.h"
#include "3D/Camera.h"

using namespace std;
using namespace DirectX;

ModelRenderer::ModelRenderer(Dx12Wrapper& dx12): dx12_(dx12)
{
	Init();
}

ModelRenderer::~ModelRenderer()
{
}

bool ModelRenderer::CreateModelPL()
{
	// ���[�g�V�O�l�`���̍쐬
	auto& sl = Application::Instance().GetShaderLoader();
	auto vertexShader	= sl.GetShader(L"Resource/Source/Shader/3D/ModelVS.hlsl", "VS", ("vs_" + sl.GetShaderModel()).c_str());
	auto pixelShader	= sl.GetShader(L"Resource/Source/Shader/3D/ModelPS.hlsl", "PS", ("ps_" + sl.GetShaderModel()).c_str());

	CreateRootSignatureFromShader(&dx12_.GetDevice(), modelRS_, vertexShader);

	//���_���C�A�E�g(�d�l)
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] =
	{
		// ���W
		{
			"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		// �@��
		{
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},

		// UV
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		// �{�[���ԍ�
		{
			"BONENO", 0,DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		// �e���x
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT ,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//���[�g�V�O�l�`���ƒ��_���C�A�E�g�̐ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
	gpsd.pRootSignature = modelRS_.Get();
	gpsd.InputLayout.pInputElementDescs = inputLayoutDescs;
	gpsd.InputLayout.NumElements = _countof(inputLayoutDescs);

	// �V�F�[�_�n
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());


	gpsd.DepthStencilState.DepthEnable = true;
	gpsd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpsd.DepthStencilState.StencilEnable = false;
	gpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���X�^���C�U
	gpsd.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// �u�����h�X�e�[�g
	gpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsd.BlendState.AlphaToCoverageEnable = false;
	gpsd.BlendState.IndependentBlendEnable = false;

	// �����_�[�^�[�Q�b�g
	gpsd.NumRenderTargets = 3;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsd.BlendState.RenderTarget[0].BlendEnable = true;
	gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	gpsd.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].LogicOpEnable = false;
	gpsd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	for (int i = 1; i < gpsd.NumRenderTargets; ++i)
	{
		gpsd.RTVFormats[i] = gpsd.RTVFormats[0];
		gpsd.BlendState.RenderTarget[i] = gpsd.BlendState.RenderTarget[0];
	}

	//���̑�
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//�O�p�`
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	auto& dev = dx12_.GetDevice();
	if (FAILED(dev.CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(modelPL_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// �V�F�[�_�n
	vertexShader	= sl.GetShader(L"Resource/Source/Shader/3D/ModelVS.hlsl", "ShadowVS", ("vs_" + sl.GetShaderModel()).c_str());
	pixelShader		= sl.GetShader(L"Resource/Source/Shader/3D/ModelPS.hlsl", "ShadowPS", ("ps_" + sl.GetShaderModel()).c_str());
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// �����_�[�^�[�Q�b�g
	for (size_t j = 0; j < gpsd.NumRenderTargets; j++)
	{
		gpsd.RTVFormats[j] = DXGI_FORMAT_UNKNOWN;
	}
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (FAILED(dev.CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(shadowPL_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	return true;
}

bool ModelRenderer::Init()
{
	// pmx�p�C�v���C���X�e�[�g�̍쐬
	if (!CreateModelPL())
	{
		assert(false);
		return false;
	}
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/����/����.pmx", _dx12, *this, GetVMDMotion("Resource/VMD/swing2.vmd")));
	modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/�ۂ�Ւ����_��/�ۂ�Ւ����_��.pmx", dx12_, *this, GetVMDMotion("Resource/VMD/swing2.vmd")));
	modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/�ۂ�Ւ������J/�ۂ�Ւ������J.pmx", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/prinzeugen/prinzeugen.pmx", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/�ݎ��~�N����/�ݎ��~�N����(�f��).pmx", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/�ۂ�Ւ�������/�ۂ�Ւ�������.pmx", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/����/����.pmd", _dx12, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/����/����.pmd", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	
	for (int j = 0; j < modelActors_.size(); j++)
	{
		int moveZ = (j + 1) / 2;
		int moveX = moveZ * ((j % 2) * 2 - 1);

		auto trans = modelActors_[j]->GetTransform();
		trans.pos = XMFLOAT3(8.0f * moveX, 0.0f, 5.0f * moveZ);
		modelActors_[j]->SetTransform(trans);
		modelActors_[j]->StartAnimation();
	}

	return true;
}

void ModelRenderer::Update()
{
	BYTE keycodeTbl[256];
	auto result = GetKeyboardState(keycodeTbl);

	auto move = [&](float& target, const unsigned int keycode, const float speed)
	{
		// �ŏ�ʃr�b�g�������Ă��邩��\���Ă���̂�128��&���Z���s��
		if (keycodeTbl[keycode] & 0x80)
		{
			target += speed;
		}
	};

	for (auto& actor : modelActors_)
	{
		actor->Update();
	}
}

void ModelRenderer::Draw()
{
	auto& texLoader = dx12_.GetTexLoader();
	auto& commandList = dx12_.GetCommand().CommandList();

	commandList.SetPipelineState(modelPL_.Get());
	commandList.SetGraphicsRootSignature(modelRS_.Get());

	// �e�N�X�`���z��̃Z�b�g
	auto& texHeap = texLoader.GetTextureHeap();
	commandList.SetDescriptorHeaps(1, texHeap.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(0, texHeap->GetGPUDescriptorHandleForHeapStart());

	// �J����
	dx12_.GetCamera().SetCameraDescriptorHeap(1);

	// �[�x
	texLoader.SetDepthTexDescriptorHeap(2, TexLoader::DepthType::camera);

	for (auto& actor : modelActors_)
	{
		actor->Draw();
	}
}

void ModelRenderer::DrawShadow()
{
	auto& texLoader = dx12_.GetTexLoader();
	auto& commandList = dx12_.GetCommand().CommandList();

	commandList.SetPipelineState(shadowPL_.Get());
	commandList.SetGraphicsRootSignature(modelRS_.Get());

	dx12_.GetCamera().SetCameraDescriptorHeap(2); 
	texLoader.SetTextureDescriptorHeap(0);

	for (auto& actor : modelActors_)
	{
		actor->Draw();
	}
}

void ModelRenderer::SetModelRS()
{
	dx12_.GetCommand().CommandList().SetGraphicsRootSignature(modelRS_.Get());
}

void ModelRenderer::SetModelPL()
{
	dx12_.GetCommand().CommandList().SetPipelineState(modelPL_.Get());
}

VMDMotion & ModelRenderer::GetVMDMotion(std::string motionPath)
{
	if (vmdMotions_.find(motionPath) == vmdMotions_.end())
	{
		vmdMotions_.emplace(motionPath, make_shared<VMDMotion>(motionPath));
	}
	return *vmdMotions_[motionPath];
}