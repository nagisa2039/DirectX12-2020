#include "PrimitiveRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "PlaneMesh.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "Utility/Constant.h"
#include "System/Application.h"
#include "System/ShaderLoader.h"
#include "Utility/dx12Tool.h"

using namespace std;
using namespace DirectX;

bool PrimitiveRenderer::CreatePipelineState()
{
	auto& sl = Application::Instance().GetShaderLoader();
	ComPtr<ID3DBlob> vertexShader = sl.GetShader(L"Resource/Source/Shader/3D/PrimitiveVS.hlsl", "VS", "vs_5_1");
	ComPtr<ID3DBlob> pixelShader = sl.GetShader(L"Resource/Source/Shader/3D/PrimitivePS.hlsl", "PS", "ps_5_1");

	CreateRootSignatureFromShader(&_dx12.GetDevice(), _primRS, vertexShader);

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
	gpsd.pRootSignature = _primRS.Get();
	gpsd.InputLayout.pInputElementDescs = inputLayoutDescs;
	// 配列の要素数を格納
	gpsd.InputLayout.NumElements = _countof(inputLayoutDescs);

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// レンダーターゲット
	gpsd.NumRenderTargets = 3;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsd.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsd.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

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

	// レンダーターゲット数
	gpsd.BlendState.RenderTarget[0].BlendEnable = true;
	gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	gpsd.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].LogicOpEnable = false;
	gpsd.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//その他
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//三角形
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	if (FAILED(_dx12.GetDevice().CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_primPL.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	vertexShader = sl.GetShader(L"Resource/Source/Shader/3D/PrimitiveVS.hlsl", "ShadowVS", "vs_5_1");
	pixelShader  = sl.GetShader(L"Resource/Source/Shader/3D/PrimitivePS.hlsl", "ShadowPS", "ps_5_1");

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// レンダーターゲット
	gpsd.NumRenderTargets = 0;
	gpsd.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	gpsd.RTVFormats[1] = DXGI_FORMAT_UNKNOWN;
	gpsd.RTVFormats[2] = DXGI_FORMAT_UNKNOWN;

	if (FAILED(_dx12.GetDevice().CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_primShadowPL.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	return true;
}

PrimitiveRenderer::PrimitiveRenderer(Dx12Wrapper& dx12):_dx12(dx12)
{
	if(!CreatePipelineState())
	{
		assert(false);
	}
	_primitives.emplace_back(make_shared<PlaneMesh>(_dx12, XMFLOAT3(0.0f, 0.0f, 0.0f), (1000.0f/536.0f) * 80.0f, 80.0f, L"image/fiona.png"));

	//_primitives.emplace_back(make_shared<ConeMesh>(_dx12, XMFLOAT3(10.0f, 0.0f, -10.0f), 5.0f, 10.0f, L"image/so.png"));
	//_primitives.emplace_back(make_shared<CylinderMesh>(_dx12, XMFLOAT3(-10.0f, 0.0f, 0.0f), 5.0f, 10.0f, L"image/misaki.png"));
}


PrimitiveRenderer::~PrimitiveRenderer()
{
}

void PrimitiveRenderer::Update()
{
	for (auto& prim : _primitives)
	{
		prim->Update();
	}
}

void PrimitiveRenderer::Draw()
{
	auto& cmdList = _dx12.GetCommand().CommandList();
	auto& texLoader = _dx12.GetTexLoader();

	cmdList.SetPipelineState(_primPL.Get());
	cmdList.SetGraphicsRootSignature(_primRS.Get());
	_dx12.SetCameraDescriptorHeap(0);

	for (auto& prim : _primitives)
	{
		prim->Draw();
	}
}

void PrimitiveRenderer::DrawShadow()
{
	auto& cmdList = _dx12.GetCommand().CommandList();
	auto& texLoader = _dx12.GetTexLoader();

	cmdList.SetPipelineState(_primShadowPL.Get());
	cmdList.SetGraphicsRootSignature(_primRS.Get());
	_dx12.SetCameraDescriptorHeap(0);

	for (auto& actor : _primitives)
	{
		actor->Draw();
	}
}
