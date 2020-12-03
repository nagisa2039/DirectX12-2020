#include "StaticMeshRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "PlaneMesh.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "Utility/Constant.h"
#include "System/Application.h"
#include "System/ShaderLoader.h"
#include "Utility/dx12Tool.h"
#include "3D/Camera.h"
#include "3D/Mesh.h"

using namespace std;
using namespace DirectX;

bool StaticMeshRenderer::CreatePipelineState()
{
	auto& sl = Application::Instance().GetShaderLoader();
	ComPtr<ID3DBlob> vertexShader = sl.GetShader(L"Resource/Source/Shader/3D/PrimitiveVS.hlsl", "VS",	("vs_" + sl.GetShaderModel()).c_str());
	ComPtr<ID3DBlob> pixelShader = sl.GetShader(L"Resource/Source/Shader/3D/PrimitivePS.hlsl", "PS",	("ps_" + sl.GetShaderModel()).c_str());

	CreateRootSignatureFromShader(&dx12_.GetDevice(), primRS_, vertexShader);

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
	};

	//ルートシグネチャと頂点レイアウトの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
	gpsd.pRootSignature = primRS_.Get();
	gpsd.InputLayout.pInputElementDescs = inputLayoutDescs;
	// 配列の要素数を格納
	gpsd.InputLayout.NumElements = _countof(inputLayoutDescs);

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// レンダーターゲット
	gpsd.NumRenderTargets = 3;
	// レンダーターゲット数
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

	gpsd.DepthStencilState.DepthEnable = true;
	gpsd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpsd.DepthStencilState.StencilEnable = false;
	gpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	for (unsigned int i = 1; i < gpsd.NumRenderTargets; ++i)
	{
		gpsd.RTVFormats[i] = gpsd.RTVFormats[0];
		gpsd.BlendState.RenderTarget[i] = gpsd.BlendState.RenderTarget[0];
	}

	// ラスタライザ
	gpsd.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// ブレンドステート
	gpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsd.BlendState.AlphaToCoverageEnable = false;
	gpsd.BlendState.IndependentBlendEnable = false;


	//その他
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//三角形
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	if (FAILED(dx12_.GetDevice().CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(primPL_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	vertexShader = sl.GetShader(L"Resource/Source/Shader/3D/PrimitiveVS.hlsl", "ShadowVS", ("vs_" + sl.GetShaderModel()).c_str());
	pixelShader  = sl.GetShader(L"Resource/Source/Shader/3D/PrimitivePS.hlsl", "ShadowPS", ("ps_" + sl.GetShaderModel()).c_str());

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// レンダーターゲット
	for (UINT i = 0; i < gpsd.NumRenderTargets; ++i)
	{
		gpsd.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	}
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (FAILED(dx12_.GetDevice().CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(primShadowPL_.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	return true;
}

StaticMeshRenderer::StaticMeshRenderer(Dx12Wrapper& dx12, std::shared_ptr<Camera>& camera)
	: dx12_(dx12), Renderer(camera)
{
	if(!CreatePipelineState())
	{
		assert(false);
	}
}


StaticMeshRenderer::~StaticMeshRenderer()
{
}

void StaticMeshRenderer::Draw(std::vector<Mesh*>& meshs)
{
	auto& cmdList = dx12_.GetCommand().CommandList();
	auto& texLoader = dx12_.GetTexLoader();

	cmdList.SetPipelineState(primPL_.Get());
	cmdList.SetGraphicsRootSignature(primRS_.Get());

	texLoader.SetTextureDescriptorHeap(0);
	camera_->SetCameraDescriptorHeap(1);
	texLoader.SetDepthTexDescriptorHeap(2, TexLoader::DepthType::camera);

	for (auto& mesh : meshs)
	{
		mesh->Draw();
	}
}

void StaticMeshRenderer::DrawShadow(std::vector<Mesh*>& meshs)
{
	auto& cmdList = dx12_.GetCommand().CommandList();
	auto& texLoader = dx12_.GetTexLoader();

	cmdList.SetPipelineState(primShadowPL_.Get());
	cmdList.SetGraphicsRootSignature(primRS_.Get());

	camera_->SetCameraDescriptorHeap(1);

	for (auto& mesh : meshs)
	{
		mesh->Draw();
	}
}
