#include <DirectXTex.h>
#include "Drawer.h"
#include "Application.h"
#include "Command.h"
#include "TexLoader.h"
#include <d3dcompiler.h>
#include "Utility/dx12Tool.h"
#include "Command.h"

using namespace DirectX;
using namespace std;

Drawer::Drawer(ID3D12Device& dev, Command& cmd, TexLoader& texLoader): _dev(dev), _cmd(cmd), _texLoader(texLoader)
{
	// 頂点の作成
	CreateVertextBuffer();

	// ルートシグネチャの作成
	CreateRootSignature();

	// パイプラインの作成
	CreatePiplineState();

	CreateVertexConstantBuffer();

	CreateSpriteHeap();

	_drawImages.clear();
}

Drawer::~Drawer()
{
}

void Drawer::CreatePiplineState()
{
	//頂点レイアウト(仕様)
	D3D12_INPUT_ELEMENT_DESC inputLayoutDescs[] =
	{
		// 座標
		{
			"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		// UV
		{
			"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//ルートシグネチャと頂点レイアウトの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
	gpsd.pRootSignature = _rootSignature.Get();
	gpsd.InputLayout.pInputElementDescs = inputLayoutDescs;
	// 配列の要素数を格納
	gpsd.InputLayout.NumElements = _countof(inputLayoutDescs);

	ComPtr<ID3DBlob> vertexShader = nullptr;
	ComPtr<ID3DBlob> pixelShader = nullptr;
	ComPtr<ID3DBlob> erBlob = nullptr;

	H_ASSERT(D3DCompileFromFile(L"Resource/Source/Shader/2DStanderd.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, vertexShader.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf()));

	H_ASSERT(D3DCompileFromFile(L"Resource/Source/Shader/2DStanderd.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", D3DCOMPILE_DEBUG |
		D3DCOMPILE_SKIP_OPTIMIZATION, 0, pixelShader.ReleaseAndGetAddressOf(), nullptr));

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

	// レンダーターゲット
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 深度ステンシル
	gpsd.DepthStencilState.DepthEnable = true;
	gpsd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	gpsd.DepthStencilState.StencilEnable = false;
	gpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ラスタライザ
	gpsd.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// ブレンドステート
	gpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsd.BlendState.AlphaToCoverageEnable = true;
	gpsd.BlendState.IndependentBlendEnable = false;

	//その他
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//三角形
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	H_ASSERT(_dev.CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf())));
}

void Drawer::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE range[2] = {};
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range[1].BaseShaderRegister = 0;
	range[1].NumDescriptors = 1;
	range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rp[2] = {};
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp[0].DescriptorTable.NumDescriptorRanges = 1;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];

	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rp[1].DescriptorTable.NumDescriptorRanges = 1;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];

	D3D12_STATIC_SAMPLER_DESC sampler[1] = {};
	sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	sampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	sampler[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	sampler[0].MaxLOD = D3D12_FLOAT32_MAX;

	D3D12_ROOT_SIGNATURE_DESC rsd = {};
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsd.NumParameters = _countof(rp);
	rsd.pParameters = rp;
	rsd.NumStaticSamplers = _countof(sampler);
	rsd.pStaticSamplers = sampler;

	ComPtr<ID3DBlob> signature = nullptr;
	ComPtr<ID3DBlob> error = nullptr;

	H_ASSERT(D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1,
		signature.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()));

	H_ASSERT(_dev.CreateRootSignature(0,
		signature->GetBufferPointer(), signature->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf())));
}

void Drawer::CreateVertextBuffer()
{
	struct Vertex_t
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	auto wsize = Application::Instance().GetWindowSize();
	Vertex_t vertices[] =
	{
		{{-1.0f,  1.0f, 0.0f},	{0, 0} }, // 左上
		{{-1.0f, -1.0f, 0.0f},	{0, 1} }, // 左下
		{{ 1.0f,  1.0f, 0.0f},	{1, 0} }, // 右上
		{{ 1.0f, -1.0f, 0.0f},	{1, 1} }  // 右下
	};

	auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));

	H_ASSERT(_dev.CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(_vertBuff.ReleaseAndGetAddressOf())));

	Vertex_t* vertMap = nullptr;
	H_ASSERT(_vertBuff->Map(0, nullptr, (void**)&vertMap));
	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	_vbView.BufferLocation = _vertBuff->GetGPUVirtualAddress();
	_vbView.StrideInBytes = sizeof(vertices[0]);
	_vbView.SizeInBytes = sizeof(vertices);
}

void Drawer::CreateVertexConstantBuffer()
{
	CreateDescriptorHeap(&_dev, _squareCBV, Application::Instance().GetImageMax());
	_squareCBs.resize(Application::Instance().GetImageMax());
	auto bufSize = sizeof(VertexInf);
	auto handle = _squareCBV->GetCPUDescriptorHandleForHeapStart();
	auto incrementSize = _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (auto& vertCB : _squareCBs)
	{
		CreateConstantBuffer(&_dev, vertCB.resorce, bufSize);
		H_ASSERT(vertCB.resorce->Map(0, nullptr, (void**)&vertCB.mappedVertexInf));
		CreateConstantBufferView(&_dev, vertCB.resorce, handle);
		handle.ptr += incrementSize;
	}
}

void Drawer::SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const float exRate, const float angle)
{
	unsigned int screenW, screenH;
	_texLoader.GetScreenSize(screenW, screenH);
	XMFLOAT2 wsizeCenter = XMFLOAT2(screenW / 2.0f, screenH / 2.0f);
	XMFLOAT2 center(left + width / 2, top + height / 2);

	float moveX = (center.x - wsizeCenter.x) / (screenW / 2.0f);
	float moveY = (center.y - wsizeCenter.y) / (screenH / 2.0f) * -1.0f;

	posTrans = XMMatrixTransformation2D(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, XMVectorSet(width / static_cast<float>(screenW) * exRate, height / static_cast<float>(screenH) * exRate, 1.0f, 1.0f),	// 拡縮
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), angle,	// 回転
		XMVectorSet(moveX, moveY, 0.0f, 1.0f));		// 移動
}

void Drawer::SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const UINT centerX, const UINT centerY, const float exRate, const float angle)
{
	unsigned int screenW, screenH;
	_texLoader.GetScreenSize(screenW, screenH);
	
	XMFLOAT2 wsizeCenter = XMFLOAT2(screenW / 2.0f, screenH / 2.0f);
	XMFLOAT2 center(left + centerX, top + centerY);

	float moveX = (center.x - wsizeCenter.x) / (screenW / 2.0f);
	float moveY = (center.y - wsizeCenter.y) / (screenH / 2.0f) * -1.0f;

	posTrans = XMMatrixTransformation2D(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, XMVectorSet(width / static_cast<float>(screenW)* exRate, height / static_cast<float>(screenH)* exRate, 1.0f, 1.0f),	// 拡縮
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), angle,	// 回転
		XMVectorSet(moveX, moveY, 0.0f, 1.0f));		// 移動
}

void Drawer::SetUVTrans(DirectX::XMMATRIX& uvTrans, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const DirectX::Image& img)
{
	XMFLOAT2 imgSizeF = XMFLOAT2(static_cast<float>(img.width), static_cast<float>(img.height));
	XMFLOAT2 uvCenter = XMFLOAT2(0.5f, 0.5f);
	XMFLOAT2 rectCenter = XMFLOAT2((srcX + width / 2) / imgSizeF.x, (srcY + height / 2) / imgSizeF.y);
	XMUINT2	 rectSizs = XMUINT2(width, height);

	uvTrans = XMMatrixTransformation2D(
		XMVectorSet(uvCenter.x, uvCenter.y, 0.0f, 0.0f), 0.0f, XMVectorSet(rectSizs.x / imgSizeF.x, rectSizs.y / imgSizeF.y, 1.0f, 1.0f),	// 拡縮
		XMVectorSet(uvCenter.x, uvCenter.y, 0.0f, 0.0f), 0.0f,	// 回転
		XMVectorSet(rectCenter.x - uvCenter.x, rectCenter.y - uvCenter.y, 0.0f, 1.0f));		// 移動
}

void Drawer::SetDefaultViewAndScissor()
{
	unsigned int screenW, screenH;
	_texLoader.GetScreenSize(screenW, screenH);

	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
	_viewport.Width = screenW;
	_viewport.Height = screenH;
	_viewport.MaxDepth = 1.0f;
	_viewport.MinDepth = 0.0f;

	_scissorRect.left = 0;
	_scissorRect.top = 0;
	_scissorRect.right = screenW;
	_scissorRect.bottom = screenH;

	auto& commandList = _cmd.CommandList();
	commandList.RSSetViewports(1, &_viewport);
	commandList.RSSetScissorRects(1, &_scissorRect);
}

void Drawer::CreateSpriteHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NodeMask = 0;
	H_ASSERT(_dev.CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(_spriteFontHeap.ReleaseAndGetAddressOf())))
}

void Drawer::End()
{
	auto& cmdList = _cmd.CommandList();

	SetDefaultViewAndScissor();

	cmdList.SetPipelineState(_pipelineState.Get());
	cmdList.SetGraphicsRootSignature(_rootSignature.Get());

	cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList.IASetVertexBuffers(0, 1, &_vbView);

	auto& texHeap = _texLoader.GetTextureHeap();
	auto sqHandle = _squareCBV->GetGPUDescriptorHandleForHeapStart();
	auto incSize = _dev.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int i = 0; i < _drawImages.size(); i++)
	{
		(*_squareCBs[i].mappedVertexInf) = _drawImages[i].vertexInf;

		cmdList.SetDescriptorHeaps(1, texHeap.GetAddressOf());
		cmdList.SetGraphicsRootDescriptorTable(0, _drawImages[i].texHandle);

		cmdList.SetDescriptorHeaps(1, _squareCBV.GetAddressOf());
		cmdList.SetGraphicsRootDescriptorTable(1, sqHandle);
		sqHandle.ptr += incSize;

		cmdList.DrawInstanced(4, 1, 0, 0);
	}

	_drawImages.clear();

	_cmd.Execute();
}

bool Drawer::DrawGraph(const INT x, const INT y, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = _texLoader.GetTextureResouse(graphHandle);

	DrawImage drawImage;
	drawImage.texHandle = texRes.gpuHandleForTex;

	SetPosTrans(drawImage.vertexInf.posTans, x, y, texRes.imageInf.width, texRes.imageInf.height);
	drawImage.vertexInf.uvTrans = XMMatrixIdentity();

	_drawImages.emplace_back(drawImage);

	return true;
}

bool Drawer::DrawRotaGraph(const INT x, const INT y, const float exRate, const float angle, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = _texLoader.GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.texHandle = texRes.gpuHandleForTex;

	SetPosTrans(drawImage.vertexInf.posTans, x - img.width / 2, y - img.height / 2, img.width, img.height, exRate, angle);
	drawImage.vertexInf.uvTrans = XMMatrixIdentity();

	_drawImages.emplace_back(drawImage);

	return true;
}

bool Drawer::DrawRotaGraph2(const INT x, const INT y, const UINT centerX, const UINT centerY, const float exRate, const float angle, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = _texLoader.GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.texHandle = texRes.gpuHandleForTex;

	SetPosTrans(drawImage.vertexInf.posTans, x - img.width / 2, y - img.height / 2, img.width, img.height, centerX, centerY, exRate, angle);
	drawImage.vertexInf.uvTrans = XMMatrixIdentity();

	_drawImages.emplace_back(drawImage);

	return true;
}

bool Drawer::DrawRectGraph(const INT destX, const INT destY, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = _texLoader.GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.texHandle = texRes.gpuHandleForTex;

	SetPosTrans(drawImage.vertexInf.posTans, destX, destY, width, height);
	SetUVTrans(drawImage.vertexInf.uvTrans, srcX, srcY, width, height, img);

	_drawImages.emplace_back(drawImage);

	return true;
}

bool Drawer::DrawExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = _texLoader.GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.texHandle = texRes.gpuHandleForTex;

	SetPosTrans(drawImage.vertexInf.posTans, left, top, right - left, buttom - top);
	drawImage.vertexInf.uvTrans = XMMatrixIdentity();

	_drawImages.emplace_back(drawImage);

	return true;
}

bool Drawer::DrawRectExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = _texLoader.GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.texHandle = texRes.gpuHandleForTex;

	SetPosTrans(drawImage.vertexInf.posTans, left, top, right - left, buttom - top);
	SetUVTrans(drawImage.vertexInf.uvTrans, srcX, srcY, width, height, img);

	_drawImages.emplace_back(drawImage);

	return true;
}