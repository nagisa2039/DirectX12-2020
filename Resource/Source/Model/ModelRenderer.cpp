#include "ModelRenderer.h"
#include <cassert>
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "ModelActor.h"
#include "Utility/Tool.h"
#include "System/Dx12Wrapper.h"
#include "System/Command.h"
#include "VMDMotion.h"
#include "System/Application.h"
#include "System/TexLoader.h"
#include "System/SpriteDrawer.h"
#include "Utility/Constant.h"
#include "Utility/dx12Tool.h"

using namespace std;
using namespace DirectX;

ModelRenderer::ModelRenderer(Dx12Wrapper& dx12): dx12_(dx12)
{
	Init();
}

ModelRenderer::~ModelRenderer()
{
}

bool ModelRenderer::CreateModelRS()
{

	return true;
}

void ModelRenderer::CreateRSRootParameter(std::vector<D3D12_ROOT_PARAMETER>& rps, std::vector<D3D12_DESCRIPTOR_RANGE>& ranges)
{
	rps.resize(5);
	// �}�e���A�� + �e�N�X�`��5
	rps[0] = {};
	rps[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rps[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rps[0].DescriptorTable.NumDescriptorRanges = 2;
	rps[0].DescriptorTable.pDescriptorRanges = &ranges[0];

	// �J����
	rps[1] = {};
	rps[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rps[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rps[1].DescriptorTable.NumDescriptorRanges = 1;
	rps[1].DescriptorTable.pDescriptorRanges = &ranges[2];

	// ���W+�{�[��
	rps[2] = rps[1];
	rps[2].DescriptorTable.pDescriptorRanges = &ranges[3];

	// �[�x�e�N�X�`��
	rps[3] = rps[1];
	rps[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rps[3].DescriptorTable.pDescriptorRanges = &ranges[4];

	// �Z�b�e�B���O���
	rps[4] = rps[1];
	rps[4].DescriptorTable.pDescriptorRanges = &ranges[5];
}

void ModelRenderer::CreateRSDescriptorRange(std::vector<D3D12_DESCRIPTOR_RANGE>& ranges)
{
	ranges.resize(6);

	// �}�e���A��
	ranges[0] = {};
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	ranges[0].BaseShaderRegister = 0;	//0
	ranges[0].NumDescriptors = 1;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	ranges[0].RegisterSpace = 0;

	// �e�N�X�`��
	ranges[1] = {};
	ranges[1].NumDescriptors = 5;
	ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// t
	ranges[1].BaseShaderRegister = 0;	//0
	ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// �J����
	ranges[2] = {};
	ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	ranges[2].BaseShaderRegister = 1;		//1
	ranges[2].NumDescriptors = 1;
	ranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	ranges[2].RegisterSpace = 0;

	// �����W3�͍��W + �{�[��
	ranges[3] = {};
	ranges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	ranges[3].BaseShaderRegister = 2;		//2, 3
	ranges[3].NumDescriptors = 2;
	ranges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	ranges[3].RegisterSpace = 0;

	// �[�x�e�N�X�`���p
	ranges[4] = ranges[2];
	ranges[4].NumDescriptors = 1;
	ranges[4].BaseShaderRegister = 5;

	// �Z�b�e�B���O���
	ranges[5] = ranges[3];
	ranges[5].NumDescriptors = 1;
	ranges[5].BaseShaderRegister = 4;
}

void ModelRenderer::CreateRSSampler(std::vector<D3D12_STATIC_SAMPLER_DESC>&  samplers)
{
	samplers.resize(3);
	samplers[0] = {};
	samplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplers[0].MaxAnisotropy = 0;
	samplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplers[0].MinLOD = 0.0f;
	samplers[0].ShaderRegister = 0;
	samplers[0].RegisterSpace = 0;
	samplers[0].MipLODBias = 0.0f;

	samplers[1] = samplers[0];
	samplers[1].ShaderRegister = 1;
	samplers[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	samplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	samplers[2] = samplers[1];
	samplers[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplers[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplers[2].ShaderRegister = 2;
}

bool ModelRenderer::CreateModelPL()
{
	// ���[�g�V�O�l�`���̍쐬
	ShaderCompile(L"Resource/Source/Shader/3D/ModelVS.hlsl", "VS", "vs_5_1", vertexShader_);
	ShaderCompile(L"Resource/Source/Shader/3D/ModelPS.hlsl", "PS", "ps_5_1", pixelShader_);

	CreateRootSignatureFromShader(&dx12_.GetDevice(), modelRS_, vertexShader_);

	//���_���C�A�E�g(�d�l)
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] =
	{
		// ���W
		{// �Z�}���e�B�N�X�l�[��
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
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader_.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader_.Get());

	// �����_�[�^�[�Q�b�g
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

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

	// �����_�[�^�[�Q�b�g��
	gpsd.BlendState.RenderTarget[0].BlendEnable = true;
	gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	gpsd.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].LogicOpEnable = false;
	gpsd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//gpsd.BlendState.RenderTarget[1] = gpsd.BlendState.RenderTarget[0];
	//gpsd.BlendState.RenderTarget[2] = gpsd.BlendState.RenderTarget[0];

	//���̑�
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//�O�p�`
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	if (FAILED(dx12_.GetDevice().CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(modelPL_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	//vertexShader = nullptr;
	//pixelShader = nullptr;
	//erBlob = nullptr;
	//if (FAILED(D3DCompileFromFile(L"Resource/Source/Shader/model.hlsl", nullptr, nullptr, "ShadowVS", "vs_5_0",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, vertexShader.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf())))
	//{
	//	assert(false);
	//	return false;
	//}

	//if (FAILED(D3DCompileFromFile(L"Resource/Source/Shader/model.hlsl", nullptr, nullptr, "ShadowPS", "ps_5_0",
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, pixelShader.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf())))
	//{
	//	assert(false);
	//	return false;
	//}

	//// �V�F�[�_�n
	//gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	//gpsd.PS =CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	//// �����_�[�^�[�Q�b�g
	//for (size_t j = 0; j < gpsd.NumRenderTargets; j++)
	//{
	//	gpsd.RTVFormats[j] = DXGI_FORMAT_UNKNOWN;
	//}
	//gpsd.NumRenderTargets = 1;

	//if (FAILED(_dx12.GetDevice().CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_shadowPL.ReleaseAndGetAddressOf()))))
	//{
	//	assert(false);
	//	return false;
	//}

	return true;
}

bool ModelRenderer::Init()
{
	// pmx���[�g�V�O�l�`���̐���
	if (!CreateModelRS())
	{
		assert(false);
		return false;
	}

	// pmx�p�C�v���C���X�e�[�g�̍쐬
	if (!CreateModelPL())
	{
		assert(false);
		return false;
	}
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/����/����.pmx", _dx12, *this, GetVMDMotion("Resource/VMD/swing2.vmd")));
	//modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/�ۂ�Ւ����_��/�ۂ�Ւ����_��.pmx", dx12_, *this, GetVMDMotion("Resource/VMD/swing2.vmd")));
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/�ۂ�Ւ������J/�ۂ�Ւ������J.pmx", _dx12, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/prinzeugen/prinzeugen.pmx", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/�ݎ��~�N����/�ݎ��~�N����(�f��).pmx", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	modelActors_.emplace_back(make_shared<ModelActor>("Resource/Model/�ۂ�Ւ�������/�ۂ�Ւ�������.pmx", dx12_, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/����/����.pmd", _dx12, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/����/����.pmd", _dx12, *this, GetVMDMotion("Resource/VMD/���S�R���_���X.vmd")));
	
	for (int j = 0; j < modelActors_.size(); j++)
	{
		int moveZ = (j + 1) / 2;
		int moveX = moveZ * ((j % 2) * 2 - 1);

		auto trans = modelActors_[j]->GetTransform();
		trans.pos = XMFLOAT3(8.0f * moveX, 0.0f, 5.0f * moveZ);
		modelActors_[j]->SetTransform(trans);
		modelActors_[j]->StartAnimation();
	}

	auto wsize = Application::Instance().GetWindowSize();
	screenH_ = dx12_.GetTexLoader().MakeScreen(D3D_SPACE_SCREEN, wsize.w, wsize.h);

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
	dx12_.GetSpriteDrawer().DrawGraph(0,0,screenH_);
}

void ModelRenderer::DrawTo3DSpace()
{
	auto& texLoader = dx12_.GetTexLoader();
	texLoader.SetDrawScreen(screenH_);
	texLoader.ClsDrawScreen();

	dx12_.SetDefaultViewAndScissor();

	auto& commandList = dx12_.GetCommand().CommandList();
	commandList.SetPipelineState(modelPL_.Get());
	commandList.SetGraphicsRootSignature(modelRS_.Get());
	commandList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dx12_.SetCameraDescriptorHeap(1);

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


void ModelRenderer::DrawFramShadow()
{
	// �p�C�v���C���X�e�[�g�̐ݒ�
	dx12_.GetCommand().CommandList().SetPipelineState(shadowPL_.Get());

	// �g�|���W�̐ݒ�
	dx12_.GetCommand().CommandList().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& actor : modelActors_)
	{
		actor->Draw(true);
	}
}

VMDMotion & ModelRenderer::GetVMDMotion(std::string motionPath)
{
	if (vmdMotions_.find(motionPath) == vmdMotions_.end())
	{
		vmdMotions_.emplace(motionPath, make_shared<VMDMotion>(motionPath));
	}
	return *vmdMotions_[motionPath];
}