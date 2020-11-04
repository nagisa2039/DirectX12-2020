#include "PrimitiveMng.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "PlaneMesh.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "Utility/Constant.h"

using namespace std;
using namespace DirectX;

bool PrimitiveMng::CreatePipelineState()
{
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


	ComPtr<ID3DBlob> vertexShader = nullptr;
	ComPtr<ID3DBlob> pixelShader = nullptr;
	ComPtr<ID3DBlob> erBlob = nullptr;

	// シェーダーの設定
	// 第一引数はファイルパス
	if (FAILED(D3DCompileFromFile(L"source/primitive.hlsl", nullptr, nullptr, "VS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, vertexShader.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf())))
	{
		assert(false);
		return false;
	}

	if (FAILED(D3DCompileFromFile(L"source/primitive.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3DCOMPILE_DEBUG |
		D3DCOMPILE_SKIP_OPTIMIZATION, 0, pixelShader.ReleaseAndGetAddressOf(), nullptr)))
	{
		assert(false);
		return false;
	}

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

	vertexShader = nullptr;
	pixelShader = nullptr;
	erBlob = nullptr;
	if (FAILED(D3DCompileFromFile(L"source/primitive.hlsl", nullptr, nullptr, "ShadowVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, vertexShader.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf())))
	{
		assert(false);
		return false;
	}

	if (FAILED(D3DCompileFromFile(L"source/primitive.hlsl", nullptr, nullptr, "ShadowPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, pixelShader.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf())))
	{
		assert(false);
		return false;
	}

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

bool PrimitiveMng::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE range[4] = {};
	// レンジ0はカメラ
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	range[0].BaseShaderRegister = 0;		//0
	range[0].NumDescriptors = 1;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	range[0].RegisterSpace = 0;

	// レンジ3は座標
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	range[1].BaseShaderRegister = 1;		//1
	range[1].NumDescriptors = 1;
	range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	range[1].RegisterSpace = 0;

	// 深度テクスチャ用
	range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// t
	range[2].BaseShaderRegister = 0;	//0
	range[2].NumDescriptors = 1;
	range[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	range[2].RegisterSpace = 0;

	range[3] = range[2];
	range[3].BaseShaderRegister = 1;	//0

	D3D12_ROOT_PARAMETER rp[4] = {};
	// カメラ
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[0].DescriptorTable.NumDescriptorRanges = 1;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];

	// 座標
	rp[1] = rp[0];
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];

	// 深度テクスチャ
	rp[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp[2].DescriptorTable.NumDescriptorRanges = 1;
	rp[2].DescriptorTable.pDescriptorRanges = &range[2];

	// 深度テクスチャ
	rp[3] = rp[2];
	rp[3].DescriptorTable.pDescriptorRanges = &range[3];

	D3D12_STATIC_SAMPLER_DESC sampler[1] = {};
	sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//テクスチャのUVが範囲外になったらリピート表示
	sampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	sampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sampler[0].MaxAnisotropy = 0;
	sampler[0].MaxLOD = D3D12_FLOAT32_MAX;
	sampler[0].MinLOD = 0.0f;
	sampler[0].ShaderRegister = 0;
	sampler[0].RegisterSpace = 0;
	sampler[0].MipLODBias = 0.0f;


	D3D12_ROOT_SIGNATURE_DESC rsd = {};
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsd.NumParameters = _countof(rp);
	rsd.pParameters = rp;
	rsd.NumStaticSamplers = _countof(sampler);
	rsd.pStaticSamplers = sampler;

	ComPtr<ID3D10Blob> sigBlob = nullptr;
	ComPtr<ID3D10Blob> errBlob = nullptr;

	if (FAILED(D3D12SerializeRootSignature(
		&rsd,	// デスクのポインタ
		D3D_ROOT_SIGNATURE_VERSION_1,	// バージョン
		sigBlob.ReleaseAndGetAddressOf(),	// signatureのポインタ
		errBlob.ReleaseAndGetAddressOf())))	// errorのポインタ
	{
		assert(false);
		return false;
	}

	if (FAILED(_dx12.GetDevice().CreateRootSignature(
		0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(_primRS.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}
	return true;
}

PrimitiveMng::PrimitiveMng(Dx12Wrapper& dx12):_dx12(dx12)
{
	if (!CreateRootSignature())
	{
		assert(false);
	}

	if(!CreatePipelineState())
	{
		assert(false);
	}

	_primitives.emplace_back(make_shared<PlaneMesh>(_dx12, XMFLOAT3(-20.0f, 0.1f, 0.0f), 20.0f, 20.0f, L"image/misaki.png"));

	_primitives.emplace_back(make_shared<PlaneMesh>(_dx12, XMFLOAT3(0.0f, 0.0f, 0.0f), (1000.0f/536.0f) * 80.0f, 80.0f, L"image/fiona.png"));

	//_primitives.emplace_back(make_shared<ConeMesh>(_dx12, XMFLOAT3(10.0f, 0.0f, -10.0f), 5.0f, 10.0f, L"image/so.png"));
	//_primitives.emplace_back(make_shared<CylinderMesh>(_dx12, XMFLOAT3(-10.0f, 0.0f, 0.0f), 5.0f, 10.0f, L"image/misaki.png"));
}


PrimitiveMng::~PrimitiveMng()
{
}

void PrimitiveMng::SetRSAndPL(bool shadow)
{
	auto& cmdList = _dx12.GetCommand().CommandList();
	cmdList.SetGraphicsRootSignature(_primRS.Get());
	cmdList.SetPipelineState(shadow ? _primShadowPL.Get() : _primPL.Get());
}

void PrimitiveMng::Update()
{
	for (auto& prim : _primitives)
	{
		prim->Update();
	}
}

void PrimitiveMng::Draw()
{
	auto& cmdList = _dx12.GetCommand().CommandList();
	auto& texLoader = _dx12.GetTexLoader();

	cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*texLoader.SetDrawScreen(lightScreenH_);
	texLoader.ClsDrawScreen();
	dx12_.SetDefaultViewAndScissor();

	commandList.SetPipelineState(shadowPL_.Get());
	commandList.SetGraphicsRootSignature(modelRS_.Get());
	dx12_.SetCameraDescriptorHeap(2);

	for (auto& actor : modelActors_)
	{
		actor->Draw(true);
	}*/

	texLoader.SetDrawScreen(texLoader.LoadGraph(D3D_CAMERA_VIEW_SCREEN));
	texLoader.ClsDrawScreen();

	cmdList.SetPipelineState(_primPL.Get());
	cmdList.SetGraphicsRootSignature(_primRS.Get());
	_dx12.SetCameraDescriptorHeap(0);

	for (auto& prim : _primitives)
	{
		prim->Draw();
	}
}