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

using namespace std;
using namespace DirectX;

ModelRenderer::ModelRenderer(Dx12Wrapper& dx12): _dx12(dx12)
{
	Init();
}

ModelRenderer::~ModelRenderer()
{
}

bool ModelRenderer::CreateModelRS()
{
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
	CreateRSDescriptorRange(ranges);

	std::vector<D3D12_ROOT_PARAMETER> rps;
	CreateRSRootParameter(rps, ranges);

	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
	CreateRSSampler(samplers);

	D3D12_ROOT_SIGNATURE_DESC rsd = {};
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsd.NumParameters = rps.size();
	rsd.pParameters = rps.data();
	rsd.NumStaticSamplers = samplers.size();
	rsd.pStaticSamplers = samplers.data();

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

	// からのルートシグネチャの生成
	// VRAM状にある情報をどう読むか
	if (FAILED(_dx12.GetDevice().CreateRootSignature(
		0,
		sigBlob->GetBufferPointer(),
		sigBlob->GetBufferSize(),
		IID_PPV_ARGS(_modelRS.ReleaseAndGetAddressOf()))))
	{
		assert(false);
		return false;
	}

	return true;
}

void ModelRenderer::CreateRSRootParameter(std::vector<D3D12_ROOT_PARAMETER>& rps, std::vector<D3D12_DESCRIPTOR_RANGE>& ranges)
{
	rps.resize(5);
	// カメラ
	rps[1] = {};
	rps[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rps[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rps[1].DescriptorTable.NumDescriptorRanges = 1;
	rps[1].DescriptorTable.pDescriptorRanges = &ranges[0];

	// マテリアル + テクスチャ
	rps[0] = {};
	rps[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rps[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rps[0].DescriptorTable.NumDescriptorRanges = 2;
	rps[0].DescriptorTable.pDescriptorRanges = &ranges[1];

	// 座標
	rps[2] = rps[1];
	rps[2].DescriptorTable.pDescriptorRanges = &ranges[3];

	// 深度テクスチャ
	rps[3] = rps[0];
	rps[3].DescriptorTable.NumDescriptorRanges = 1;
	rps[3].DescriptorTable.pDescriptorRanges = &ranges[4];

	// セッティング情報
	rps[4] = rps[1];
	rps[4].DescriptorTable.NumDescriptorRanges = 1;
	rps[4].DescriptorTable.pDescriptorRanges = &ranges[5];
}

void ModelRenderer::CreateRSDescriptorRange(std::vector<D3D12_DESCRIPTOR_RANGE>& ranges)
{
	ranges.resize(6);

	// レンジ0はカメラ
	ranges[0] = {};
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	ranges[0].BaseShaderRegister = 1;		//1
	ranges[0].NumDescriptors = 1;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	ranges[0].RegisterSpace = 0;

	// レンジ1マテリアル
	ranges[1] = {};
	ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	ranges[1].BaseShaderRegister = 0;	//0
	ranges[1].NumDescriptors = 1;
	ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	ranges[1].RegisterSpace = 0;

	// テクスチャ
	ranges[2] = {};
	ranges[2].NumDescriptors = 5;
	ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// t
	ranges[2].BaseShaderRegister = 0;	//0
	ranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// レンジ3は座標 + ボーン
	ranges[3] = {};
	ranges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	//b
	ranges[3].BaseShaderRegister = 2;		//2
	ranges[3].NumDescriptors = 2;
	ranges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	ranges[3].RegisterSpace = 0;

	// 深度テクスチャ用
	ranges[4] = ranges[2];
	ranges[4].NumDescriptors = 1;
	ranges[4].BaseShaderRegister = 5;

	// セッティング情報
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
	//頂点レイアウト(仕様)
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] =
	{
		// 座標
		{// セマンティクスネーム
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
	gpsd.pRootSignature = _modelRS.Get();
	gpsd.InputLayout.pInputElementDescs = inputLayoutDescs;
	gpsd.InputLayout.NumElements = _countof(inputLayoutDescs);

	ComPtr<ID3DBlob> vertexShader = nullptr;
	ComPtr<ID3DBlob> pixelShader = nullptr;
	ComPtr<ID3DBlob> erBlob = nullptr;

	// シェーダーの設定
	// 第一引数はファイルパス
	if (FAILED(D3DCompileFromFile(L"Resource/Source/Shader/model.hlsl",nullptr,nullptr,"VS","vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,0,vertexShader.ReleaseAndGetAddressOf(),erBlob.ReleaseAndGetAddressOf())))
	{
		assert(false);
		return false;
	}

	if (FAILED(D3DCompileFromFile(L"Resource/Source/Shader/model.hlsl", nullptr, nullptr, "PS", "ps_5_0", 
		D3DCOMPILE_DEBUG |D3DCOMPILE_SKIP_OPTIMIZATION, 0, pixelShader.ReleaseAndGetAddressOf(), nullptr)))
	{
		assert(false);
		return false;
	}

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// レンダーターゲット
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

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
	//gpsd.BlendState.RenderTarget[1] = gpsd.BlendState.RenderTarget[0];
	//gpsd.BlendState.RenderTarget[2] = gpsd.BlendState.RenderTarget[0];

	//その他
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//三角形
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	if (FAILED(_dx12.GetDevice().CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_modelPL.ReleaseAndGetAddressOf()))))
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

	//// シェーダ系
	//gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	//gpsd.PS =CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	//// レンダーターゲット
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
	// pmxルートシグネチャの生成
	if (!CreateModelRS())
	{
		assert(false);
		return false;
	}

	// pmxパイプラインステートの作成
	if (!CreateModelPL())
	{
		assert(false);
		return false;
	}
	_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/ぽんぷ長式神風/ぽんぷ長式神風.pmx", _dx12, *this, GetVMDMotion("Resource/VMD/swing2.vmd")));
	_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/ぽんぷ長式夕立改二/ぽんぷ長式夕立改二.pmx", _dx12, *this, GetVMDMotion("Resource/VMD/ヤゴコロダンス.vmd")));
	_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/ぽんぷ長式村雨/ぽんぷ長式村雨.pmx", _dx12, *this, GetVMDMotion("Resource/VMD/ヤゴコロダンス.vmd")));
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/斑鳩/斑鳩.pmd", _dx12, *this, GetVMDMotion("Resource/VMD/ヤゴコロダンス.vmd")));
	//_modelActors.emplace_back(make_shared<ModelActor>("Resource/Model/葛城/葛城.pmd", _dx12, *this, GetVMDMotion("Resource/VMD/ヤゴコロダンス.vmd")));
	
	for (int j = 0; j < _modelActors.size(); j++)
	{
		int moveZ = (j + 1) / 2;
		int moveX = moveZ * ((j % 2) * 2 - 1);

		auto trans = _modelActors[j]->GetTransform();
		trans.pos = XMFLOAT3(8.0f * moveX, 0.0f, 5.0f * moveZ);
		_modelActors[j]->SetTransform(trans);
		_modelActors[j]->StartAnimation();
	}

	auto wsize = Application::Instance().GetWindowSize();
	_screenH = _dx12.GetTexLoader().MakeScreen(wsize.w, wsize.h);

	return true;
}

void ModelRenderer::Update()
{
	BYTE keycodeTbl[256];
	GetKeyboardState(keycodeTbl);

	auto move = [&](float& target, const unsigned int keycode, const float speed)
	{
		// 最上位ビットが押しているかを表しているので128と&演算を行う
		if (keycodeTbl[keycode] & 0x80)
		{
			target += speed;
		}
	};

	for (auto& actor : _modelActors)
	{
		actor->Update();
	}
}

void ModelRenderer::Draw()
{
	_dx12.GetSpriteDrawer().DrawGraph(0,0,_screenH);
}

void ModelRenderer::DrawToMyScreen()
{
	auto& texLoader = _dx12.GetTexLoader();
	texLoader.SetDrawScreen(_screenH);
	texLoader.ClsDrawScreen();

	_dx12.SetDefaultViewAndScissor();

	auto& commandList = _dx12.GetCommand().CommandList();
	commandList.SetPipelineState(_modelPL.Get());
	commandList.SetGraphicsRootSignature(_modelRS.Get());
	commandList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_dx12.SetCameraDescriptorHeap(1);

	for (auto& actor : _modelActors)
	{
		actor->Draw();
	}
}

void ModelRenderer::SetModelRS()
{
	_dx12.GetCommand().CommandList().SetGraphicsRootSignature(_modelRS.Get());
}

void ModelRenderer::SetModelPL()
{
	_dx12.GetCommand().CommandList().SetPipelineState(_modelPL.Get());
}


void ModelRenderer::DrawFramShadow()
{
	// パイプラインステートの設定
	_dx12.GetCommand().CommandList().SetPipelineState(_shadowPL.Get());

	// トポロジの設定
	_dx12.GetCommand().CommandList().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& actor : _modelActors)
	{
		actor->Draw(true);
	}
}

VMDMotion & ModelRenderer::GetVMDMotion(std::string motionPath)
{
	if (_vmdMotions.find(motionPath) == _vmdMotions.end())
	{
		_vmdMotions.emplace(motionPath, make_shared<VMDMotion>(motionPath));
	}
	return *_vmdMotions[motionPath];
}