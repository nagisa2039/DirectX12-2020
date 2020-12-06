#include "SkeletalMeshRenderer.h"
#include <cassert>
#include "d3dx12.h"
#include "SkeletalMesh.h"
#include "Utility/Tool.h"
#include "System/Dx12Wrapper.h"
#include "System/Command.h"
#include "System/Application.h"
#include "System/TexLoader.h"
#include "2D/SpriteDrawer.h"
#include "Utility/Constant.h"
#include "Utility/dx12Tool.h"
#include "System/ShaderLoader.h"
#include "3D/Camera.h"
#include "3D/Mesh.h"

using namespace std;
using namespace DirectX;

SkeletalMeshRenderer::SkeletalMeshRenderer(Dx12Wrapper& dx12, std::shared_ptr<Camera>& camera)
	: dx12_(dx12), Renderer(camera)
{
	Init();
}

SkeletalMeshRenderer::~SkeletalMeshRenderer()
{
}

bool SkeletalMeshRenderer::CreateModelPL()
{
	// ルートシグネチャの作成
	auto& sl = Application::Instance().GetShaderLoader();
	auto vertexShader	= sl.GetShader(L"Resource/Source/Shader/3D/ModelVS.hlsl", "VS", ("vs_" + sl.GetShaderModel()).c_str());
	auto pixelShader	= sl.GetShader(L"Resource/Source/Shader/3D/ModelPS.hlsl", "PS", ("ps_" + sl.GetShaderModel()).c_str());

	CreateRootSignatureFromShader(&dx12_.GetDevice(), modelRS_, vertexShader);

	//頂点レイアウト(仕様)
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] =
	{
		// 座標
		{
			"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		// 法線
		{
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},

		// UV
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		// ボーン番号
		{
			"BONENO", 0,DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		// 影響度
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT ,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//ルートシグネチャと頂点レイアウトの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
	gpsd.pRootSignature = modelRS_.Get();
	gpsd.InputLayout.pInputElementDescs = inputLayoutDescs;
	gpsd.InputLayout.NumElements = _countof(inputLayoutDescs);

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());


	gpsd.DepthStencilState.DepthEnable = true;
	gpsd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpsd.DepthStencilState.StencilEnable = false;
	gpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ラスタライザ
	gpsd.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// ブレンドステート
	gpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsd.BlendState.AlphaToCoverageEnable = false;
	gpsd.BlendState.IndependentBlendEnable = false;

	// レンダーターゲット
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

	for (unsigned int i = 1; i < gpsd.NumRenderTargets; ++i)
	{
		gpsd.RTVFormats[i] = gpsd.RTVFormats[0];
		gpsd.BlendState.RenderTarget[i] = gpsd.BlendState.RenderTarget[0];
	}

	//その他
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//三角形
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	auto& dev = dx12_.GetDevice();
	if (FAILED(dev.CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(modelPL_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	// シェーダ系
	vertexShader	= sl.GetShader(L"Resource/Source/Shader/3D/ModelVS.hlsl", "ShadowVS", ("vs_" + sl.GetShaderModel()).c_str());
	pixelShader		= sl.GetShader(L"Resource/Source/Shader/3D/ModelPS.hlsl", "ShadowPS", ("ps_" + sl.GetShaderModel()).c_str());
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// レンダーターゲット
	for (size_t j = 0; j < gpsd.NumRenderTargets; j++)
	{
		gpsd.RTVFormats[j] = DXGI_FORMAT_UNKNOWN;
	}
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (FAILED(dev.CreateGraphicsPipelineState(&gpsd, 
		IID_PPV_ARGS(shadowPL_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	return true;
}

bool SkeletalMeshRenderer::CreateCompute()
{
	auto& dev = dx12_.GetDevice();
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	auto& sl = Application::Instance().GetShaderLoader();
	auto cs = sl.GetShader(L"Resource/Source/Shader/3D/ModelCS.hlsl", 
		"CS", ("cs_" + sl.GetShaderModel()).c_str());
	
	CreateRootSignatureFromShader(&dev, computeRS_, cs);
	
	desc.CS = CD3DX12_SHADER_BYTECODE(cs.Get());;
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.pRootSignature = computeRS_.Get();

	H_ASSERT(dev.CreateComputePipelineState(&desc, 
		IID_PPV_ARGS(computePL_.ReleaseAndGetAddressOf())));
	return true;
}

bool SkeletalMeshRenderer::Init()
{
	// pmxパイプラインステートの作成
	CreateModelPL();

	CreateCompute();

	return true;
}

void SkeletalMeshRenderer::ComputeUpdate(std::vector<Mesh*>& meshs)
{
	auto& commandList = dx12_.GetCommand().CommandList();
	commandList.SetComputeRootSignature(computeRS_.Get());
	commandList.SetPipelineState(computePL_.Get());
	for (auto& mesh : meshs)
	{
		mesh->ComputeUpdate();
	}
}

void SkeletalMeshRenderer::Draw(std::vector<Mesh*>& meshs)
{
	auto& texLoader = dx12_.GetTexLoader();
	auto& commandList = dx12_.GetCommand().CommandList();

	commandList.SetPipelineState(modelPL_.Get());
	commandList.SetGraphicsRootSignature(modelRS_.Get());

	// テクスチャ配列のセット
	auto& texHeap = texLoader.GetTextureHeap();
	commandList.SetDescriptorHeaps(1, texHeap.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(0, texHeap->GetGPUDescriptorHandleForHeapStart());

	// カメラ
	camera_->SetCameraDescriptorHeap(1);

	// 深度
	texLoader.SetDepthTexDescriptorHeap(2, TexLoader::DepthType::camera);

	for (auto& mesh : meshs)
	{
		mesh->Draw();
	}
}

void SkeletalMeshRenderer::DrawShadow(std::vector<Mesh*>& meshs)
{
	auto& texLoader = dx12_.GetTexLoader();
	auto& commandList = dx12_.GetCommand().CommandList();

	commandList.SetPipelineState(shadowPL_.Get());
	commandList.SetGraphicsRootSignature(modelRS_.Get());

	camera_->SetCameraDescriptorHeap(1);
	texLoader.SetTextureDescriptorHeap(0);

	for (auto& mesh : meshs)
	{
		mesh->Draw();
	}
}

void SkeletalMeshRenderer::SetModelRS()
{
	dx12_.GetCommand().CommandList().SetGraphicsRootSignature(modelRS_.Get());
}

void SkeletalMeshRenderer::SetModelPL()
{
	dx12_.GetCommand().CommandList().SetPipelineState(modelPL_.Get());
}