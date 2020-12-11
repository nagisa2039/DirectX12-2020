#include "SpriteDrawer.h"
#include "System/Application.h"
#include "System/Command.h"
#include "System/TexLoader.h"
#include "Utility/dx12Tool.h"
#include "System/Command.h"
#include "System/Dx12Wrapper.h"
#include "Utility/Cast.h"
#include "System/ShaderLoader.h"
#include "Material/StanderedMaterial.h"

using namespace DirectX;
using namespace std;

namespace
{
	constexpr float EMMISION_RATE = 10.0f;
}

SpriteDrawer::SpriteDrawer(Dx12Wrapper& dx12):dx12_(dx12)
{
	drawBright_ = XMFLOAT3(1.0f, 1.0f, 1.0f);
	blendValue_ = 1.0f;

	ClearDrawData();

	// 頂点の作成
	CreateVertextBuffer();

	CreateIndexBuffer();

	// ルートシグネチャの作成
	CreateRootSignature();

	// パイプラインの作成
	CreatePiplineState();

	auto& dev = dx12.GetDevice();

	const int imageMax = Application::Instance().GetImageMax();
	const int vertInfStructSize = sizeof(VerticesInf);
	CreateStructuredBufferAndHeap(&dev, verticesInfSB_.mappedVertexInf,
		verticesInfSB_.resource.buffer, verticesInfHeap_, vertInfStructSize, imageMax);
	const int pixelInfStructSize = sizeof(PixelInf);
	CreateStructuredBufferAndHeap(&dev, pixelInfSB_.mappedPixelInf,
		pixelInfSB_.resource.buffer, pixelInfHeap_, pixelInfStructSize, imageMax);

	CreateStructuredBufferAndHeap(&dev, utility_.mapped,
		utility_.resource.buffer, utilityHeap_, sizeof(*utility_.mapped), 1);
	utility_.mapped->emmisionRate = EMMISION_RATE;

	drawImages_.clear();

	SetDrawBlendMode(BlendMode::noblend, 255);
}

SpriteDrawer::~SpriteDrawer()
{
}

bool SpriteDrawer::SetMaterial(std::shared_ptr<Material> material)
{
	material_ = material;
	if (material_->GetPipelineState() == nullptr)
	{
		//ルートシグネチャと頂点レイアウトの設定
		std::vector<D3D12_INPUT_ELEMENT_DESC> iedVec;
		GetDefaultInputElementDesc(iedVec);
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
		GetDefaultPipelineStateDesc(iedVec, gpsd);
		auto& sl = Application::Instance().GetShaderLoader();
		gpsd.PS = CD3DX12_SHADER_BYTECODE(sl.GetPixelShader(material_->GetShaderPath().c_str()).Get());

		H_ASSERT(dx12_.GetDevice().CreateGraphicsPipelineState(&gpsd, 
			IID_PPV_ARGS(material_->GetPipelineState().ReleaseAndGetAddressOf())));
	}
	return true;
}

void SpriteDrawer::CreatePiplineState()
{
	//ルートシグネチャと頂点レイアウトの設定
	std::vector<D3D12_INPUT_ELEMENT_DESC> iedVec;
	GetDefaultInputElementDesc(iedVec);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};
	GetDefaultPipelineStateDesc(iedVec, gpsd);

	array<function<void()>, Uint64(BlendMode::max)> blendDescSets_;
	blendDescSets_[Uint64(BlendMode::alpha)] = [&gpsd = gpsd](){};
	blendDescSets_[Uint64(BlendMode::noblend)] = [&gpsd = gpsd]() {};

	blendDescSets_[Uint64(BlendMode::add)] = [&gpsd = gpsd]()
	{
		gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	};
	blendDescSets_[Uint64(BlendMode::sub)] = [&gpsd = gpsd]()
	{
		gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_DEST_ALPHA;
		gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
	};
	blendDescSets_[Uint64(BlendMode::mula)] = [&gpsd = gpsd]()
	{
		gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	};
	blendDescSets_[Uint64(BlendMode::inv)] = [&gpsd = gpsd]()
	{
		gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
		gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	};
	
	for (int i = 0; auto & material : standeredBlendPipelineStates_)
	{
		blendDescSets_[i]();
		material = make_shared<StanderedMaterial>(L"Resource/Source/Shader/2D/2DStanderd.hlsl");
		H_ASSERT(dx12_.GetDevice().CreateGraphicsPipelineState(&gpsd, 
			IID_PPV_ARGS(material->GetPipelineState().ReleaseAndGetAddressOf())));
		i++;
	}
}

void SpriteDrawer::CreateRootSignature()
{
	// シェーダーコンパイル
	auto sl = Application::Instance().GetShaderLoader();
	vertexShader_	= sl.GetShader(L"Resource/Source/Shader/2D/2DStanderdVS.hlsl", "VS", ("vs_" + sl.GetShaderModel()).c_str());
	pixelShader_	= sl.GetShader(L"Resource/Source/Shader/2D/2DStanderdPS.hlsl", "PS", ("ps_" + sl.GetShaderModel()).c_str());

	CreateRootSignatureFromShader(&dx12_.GetDevice(), rootSignature_, vertexShader_);
}

void SpriteDrawer::CreateVertextBuffer()
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

	auto size = sizeof(vertices);
	CreateBuffer(&dx12_.GetDevice(), vertResource_.buffer, D3D12_HEAP_TYPE_UPLOAD, size);

	Vertex_t* vertMap = nullptr;
	MapAndCopy(vertMap, vertResource_, begin(vertices), end(vertices));

	vbView_.BufferLocation = vertResource_.buffer->GetGPUVirtualAddress();
	vbView_.StrideInBytes = Uint32(size / _countof(vertices));
	vbView_.SizeInBytes = Uint32(size);

}

void SpriteDrawer::CreateIndexBuffer()
{
	std::vector<uint16_t> indices = { 0, 2, 1, 1, 2, 3 };
	auto size = Uint64(sizeof(indices[0]) * indices.size());
	CreateBuffer(&dx12_.GetDevice(), indexResource_.buffer, D3D12_HEAP_TYPE_UPLOAD, size);

	uint16_t* indexMap = nullptr;
	MapAndCopy(indexMap, indexResource_, indices.begin(), indices.end());

	ibView_.BufferLocation = indexResource_.buffer->GetGPUVirtualAddress();
	ibView_.SizeInBytes = Uint32(size);
	ibView_.Format = DXGI_FORMAT_R16_UINT;
}

void SpriteDrawer::SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const float exRate, const float angle)
{
	SetPosTrans(posTrans, left, top, width, height, width / 2, height / 2, exRate, angle);
}

void SpriteDrawer::SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const UINT centerX, const UINT centerY, const float exRate, const float angle)
{
	unsigned int screenW, screenH;
	dx12_.GetTexLoader().GetScreenSize(screenW, screenH);
	
	XMFLOAT2 wsizeCenter = XMFLOAT2(screenW / 2.0f, screenH / 2.0f);

	float exW = width * exRate;
	float exH = height * exRate;
	XMFLOAT2 center(Float(left + exW / 2), Float(top + exH /2));

	float moveX = (center.x - wsizeCenter.x) / (screenW / 2.0f);
	float moveY = (center.y - wsizeCenter.y) / (screenH / 2.0f) * -1.0f;

	auto scaleMat = XMMatrixScaling(exW, exH, 1.0f);
	auto rotateZMat = XMMatrixRotationZ(angle);
	auto transMat = XMMatrixTranslation(moveX, moveY, 0.0f);

	auto aspectMat = XMMatrixScaling(1.0f / Float(screenW), 1.0f / Float(screenH), 1.0f);
	auto mat1 = XMMatrixMultiply(XMMatrixMultiply(scaleMat, rotateZMat), aspectMat);

	auto mat2 = XMMatrixMultiply(mat1, transMat);

	posTrans = mat2;
}

void SpriteDrawer::SetUVTrans(DirectX::XMMATRIX& uvTrans, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const DirectX::Image& img)
{
	XMFLOAT2 imgSizeF = XMFLOAT2(static_cast<float>(img.width), static_cast<float>(img.height));
	XMFLOAT2 uvCenter = XMFLOAT2(0.5f, 0.5f);
	XMFLOAT2 rectCenter = XMFLOAT2((srcX + width / 2) / imgSizeF.x, (srcY + height / 2) / imgSizeF.y);
	XMUINT2	 rectSizs = XMUINT2(width, height);

	uvTrans = XMMatrixIdentity();
	uvTrans = XMMatrixTransformation2D(
		XMVectorSet(uvCenter.x, uvCenter.y, 0.0f, 0.0f), 0.0f, XMVectorSet(rectSizs.x / imgSizeF.x, rectSizs.y / imgSizeF.y, 1.0f, 1.0f),	// 拡縮
		XMVectorSet(uvCenter.x, uvCenter.y, 0.0f, 0.0f), 0.0f,	// 回転
		XMVectorSet(rectCenter.x - uvCenter.x, rectCenter.y - uvCenter.y, 0.0f, 1.0f));		// 移動
}

void SpriteDrawer::End()
{
	auto& command = dx12_.GetCommand();
	auto& cmdList = command.CommandList();

	auto& texLoader = dx12_.GetTexLoader();
	auto& texHeap = texLoader.GetTextureHeap();

	// 時間定数の更新
	utility_.mapped->time = GetTickCount64() / 1000.0f;

	// 描画
	for (size_t i = 0; const auto & drawGroup : drawGroups_)
	{
		dx12_.SetDefaultViewAndScissor();

		for (int j = 0; j < drawGroup.num; j++)
		{
			verticesInfSB_.mappedVertexInf[j] = drawImages_[i + j].verticesInf;
			pixelInfSB_.mappedPixelInf[j] = drawImages_[i + j].pixelInf;
		}
		i += drawGroup.num;

		cmdList.SetPipelineState(drawGroup.material->GetPipelineState().Get());
		cmdList.SetGraphicsRootSignature(rootSignature_.Get());

		cmdList.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList.IASetVertexBuffers(0, 1, &vbView_);
		cmdList.IASetIndexBuffer(&ibView_);

		// テクスチャ配列のセット
		cmdList.SetDescriptorHeaps(1, texHeap.GetAddressOf());
		cmdList.SetGraphicsRootDescriptorTable(0, texHeap->GetGPUDescriptorHandleForHeapStart());

		// 深度テクスチャのセット
		texLoader.SetDepthTexDescriptorHeap(2, TexLoader::DepthType::camera);

		// 定数
		cmdList.SetDescriptorHeaps(1, utilityHeap_.GetAddressOf());
		cmdList.SetGraphicsRootDescriptorTable(3, utilityHeap_->GetGPUDescriptorHandleForHeapStart());

		// マテリアルごとの定数セット
		drawGroup.material->SetEachDescriptorHeap(cmdList);

		// VS情報配列のセット
		cmdList.SetDescriptorHeaps(1, verticesInfHeap_.GetAddressOf());
		cmdList.SetGraphicsRootDescriptorTable(7, verticesInfHeap_->GetGPUDescriptorHandleForHeapStart());

		// PS情報配列のセット
		cmdList.SetDescriptorHeaps(1, pixelInfHeap_.GetAddressOf());
		cmdList.SetGraphicsRootDescriptorTable(8, pixelInfHeap_->GetGPUDescriptorHandleForHeapStart());


		cmdList.DrawIndexedInstanced(6, drawGroup.num, 0, 0, 0);

		command.Execute();

		texLoader.SetDrawScreen(texLoader.GetCurrentRenderTarget());
	}
	ClearDrawData();
}

void SpriteDrawer::ClearDrawData()
{
	drawGroups_.clear();
	drawImages_.clear();
}

void SpriteDrawer::GetDefaultInputElementDesc(std::vector<D3D12_INPUT_ELEMENT_DESC>& ied)
{
	//頂点レイアウト(仕様)
	ied =
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
}

void SpriteDrawer::GetDefaultPipelineStateDesc(const std::vector<D3D12_INPUT_ELEMENT_DESC>& ied, D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpsd)
{
	//ルートシグネチャと頂点レイアウトの設定
	gpsd = {};
	gpsd.pRootSignature = rootSignature_.Get();
	gpsd.InputLayout.pInputElementDescs = ied.data();
	// 配列の要素数を格納
	gpsd.InputLayout.NumElements = Uint32(ied.size());

	// シェーダ系
	gpsd.VS = CD3DX12_SHADER_BYTECODE(vertexShader_.Get());
	gpsd.PS = CD3DX12_SHADER_BYTECODE(pixelShader_.Get());

	// レンダーターゲット
	gpsd.NumRenderTargets = 1;
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 深度ステンシル
	gpsd.DepthStencilState.DepthEnable = false;
	gpsd.DepthStencilState.StencilEnable = false;

	// ラスタライザ
	gpsd.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	//その他
	gpsd.NodeMask = 0;
	gpsd.SampleDesc.Count = 1;
	gpsd.SampleDesc.Quality = 0;
	gpsd.SampleMask = 0xffffffff;

	//三角形
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// ブレンドステート
	gpsd.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsd.BlendState.AlphaToCoverageEnable = false;
	gpsd.BlendState.IndependentBlendEnable = false;
	gpsd.BlendState.RenderTarget[0].BlendEnable = true;

	gpsd.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	gpsd.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
}

void SpriteDrawer::SetDrawBright(const INT r, const INT g, const INT b)
{
	drawBright_.x = Saturate(r/255.0f);
	drawBright_.y = Saturate(g/255.0f);
	drawBright_.z = Saturate(b/255.0f);
}

void SpriteDrawer::SetDrawBlendMode(const BlendMode blendMode, const INT value)
{
	if (blendMode == BlendMode::noblend)
	{
		blendValue_ = 1.0f;
	}
	else
	{
		blendValue_ = Saturate(value / 255.0f);
	}
	material_ = standeredBlendPipelineStates_[Uint64(blendMode)];
}

void SpriteDrawer::SetDrawScreen(const int graphHandle)
{
	if (drawImages_.size() > 0)
	{
		End();
	}
	dx12_.GetTexLoader().SetDrawScreen(graphHandle);
}

bool SpriteDrawer::DrawGraph(const INT x, const INT y, const int graphHandle)
{
	Image img = dx12_.GetTexLoader().GetTextureResouse(graphHandle).imageInf;
	return DrawRectGraph(x, y, 0, 0, Uint32(img.width), Uint32(img.height), graphHandle);
}

bool SpriteDrawer::DrawRotaGraph(const INT x, const INT y, const float exRate, const float angle, const int graphHandle)
{
	Image img = dx12_.GetTexLoader().GetTextureResouse(graphHandle).imageInf;
	return DrawRotaGraph2(x, y, Uint32(img.width/2), Uint32(img.height/2), exRate, angle, graphHandle);
}

bool SpriteDrawer::DrawRotaGraph2(const INT x, const INT y, const UINT centerX, const UINT centerY, const float exRate, const float angle, const int graphHandle)
{
	auto& texRes = dx12_.GetTexLoader().GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;
	DrawRectRotaGraph2(x, y, 0, 0, Uint32(img.width), Uint32(img.height), centerX, centerY, exRate, angle, graphHandle);

	return true;
}

bool SpriteDrawer::DrawRectGraph(const INT destX, const INT destY, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = dx12_.GetTexLoader().GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.pixelInf.texIndex = graphHandle;

	SetPosTrans(drawImage.verticesInf.posTans, destX, destY, width, height);
	SetUVTrans(drawImage.verticesInf.uvTrans, srcX, srcY, width, height, img);

	AddDrawImage(drawImage);

	return true;
}

bool SpriteDrawer::DrawRectRotaGraph2(const INT x, const INT y, const INT srcX, const INT srcY, const INT width, const INT height, const INT cx, const INT cy, const float exRate, const float angle, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = dx12_.GetTexLoader().GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.pixelInf.texIndex = graphHandle;

	SetPosTrans(drawImage.verticesInf.posTans, x - cx, y - cy,
		Uint32(img.width), Uint32(img.height), cx, cy, exRate, angle);
	drawImage.verticesInf.uvTrans = XMMatrixIdentity();
	SetUVTrans(drawImage.verticesInf.uvTrans, srcX, srcY, width, height, img);

	AddDrawImage(drawImage);

	return true;
}

bool SpriteDrawer::DrawExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = dx12_.GetTexLoader().GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.pixelInf.texIndex = graphHandle;

	SetPosTrans(drawImage.verticesInf.posTans, left, top, right - left, buttom - top);
	drawImage.verticesInf.uvTrans = XMMatrixIdentity();

	AddDrawImage(drawImage);

	return true;
}

bool SpriteDrawer::DrawRectExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle)
{
	if (graphHandle == -1) return false;

	auto& texRes = dx12_.GetTexLoader().GetTextureResouse(graphHandle);
	Image img = texRes.imageInf;

	DrawImage drawImage;
	drawImage.pixelInf.texIndex = graphHandle;

	SetPosTrans(drawImage.verticesInf.posTans, left, top, right - left, buttom - top);
	SetUVTrans(drawImage.verticesInf.uvTrans, srcX, srcY, width, height, img);

	AddDrawImage(drawImage);

	return true;
}

void SpriteDrawer::AddDrawImage(SpriteDrawer::DrawImage& drawImage)
{
	if (drawGroups_.size() > 0 && drawGroups_.rbegin()->material == material_)
	{
		drawGroups_.rbegin()->num++;
	}
	else
	{
		drawGroups_.emplace_back(DrawGroup{ material_, 1 });
	}

	drawImage.pixelInf.bright = drawBright_;
	drawImage.pixelInf.alpha = blendValue_;
	drawImages_.emplace_back(drawImage);
}