#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>
#include<SpriteFont.h>
#include<ResourceUploadBatch.h>

class Dx12Wrapper;
using Microsoft::WRL::ComPtr;

class SpriteDrawer
{
public:
	SpriteDrawer(Dx12Wrapper& dx12);
	~SpriteDrawer();

	bool DrawGraph(const INT x, const INT y, const int graphHandle);
	bool DrawRotaGraph(const INT x, const INT y, const float exRate, const float angle, const int graphHandle);
	bool DrawRotaGraph2(const INT x, const INT y, const UINT centerX, const UINT centerY, const float exRate, const float angle, const int graphHandle);
	bool DrawRectGraph(const INT destX, const INT destY, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);
	bool DrawExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const int graphHandle);
	bool DrawRectExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);

	void End();

private:
	SpriteDrawer(const SpriteDrawer&) = delete;
	SpriteDrawer& operator=(const SpriteDrawer&) = delete;

	Dx12Wrapper& _dx12;

	struct VertexInf
	{
		DirectX::XMMATRIX posTans;
		DirectX::XMMATRIX uvTrans;
	};

	struct DrawImage
	{
		VertexInf vertexInf;
		D3D12_GPU_DESCRIPTOR_HANDLE texHandle;
	};

	struct VertexResource
	{
		ComPtr<ID3D12Resource> resorce;
		VertexInf* mappedVertexInf;
	};

	ComPtr<ID3D12Resource> _vertBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _vbView = {};

	ComPtr<ID3D12RootSignature> _rootSignature = nullptr;
	ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

	std::vector<VertexResource> _squareCBs;
	ComPtr<ID3D12DescriptorHeap> _squareCBV = nullptr;

	std::vector<DrawImage> _drawImages;

	ComPtr<ID3D12DescriptorHeap> _spriteFontHeap = nullptr;
	std::shared_ptr<DirectX::GraphicsMemory> _gmemory = nullptr;//グラフィクスメモリオブジェクト
	std::shared_ptr<DirectX::SpriteFont> _spriteFont = nullptr;//フォント表示用オブジェクト
	std::shared_ptr<DirectX::SpriteBatch> _spriteBatch = nullptr;//スプライト表示用オブジェクト

	void CreateVertexConstantBuffer();

	void CreatePiplineState();
	void CreateRootSignature();
	void CreateVertextBuffer();

	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const float exRate = 1.0f, const float angle = 0.0f);
	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const UINT centerX, const UINT centerY, const float exRate = 1.0f, const float angle = 0.0f);
	void SetUVTrans(DirectX::XMMATRIX& uvTrans, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const DirectX::Image& img);


	void CreateSpriteHeap();

};

