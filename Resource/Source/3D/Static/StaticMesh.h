#pragma once
#include "3D/Mesh.h"
#include <DirectXMath.h>
#include "Utility/ComPtr.h"
#include <d3d12.h>
#include <vector>
#include <string>
#include <memory>

class Material;

class StaticMesh :
	public Mesh
{
public:
	struct PrimVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;

		PrimVertex(): 
			pos(DirectX::XMFLOAT3(0,0,0)), 
			normal(DirectX::XMFLOAT3(0, 0, 0)),
			uv(DirectX::XMFLOAT2(0.0f, 0.0f)){};

		PrimVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 norm, DirectX::XMFLOAT2 uv) :
			pos(p), normal(norm), uv(uv){};

		PrimVertex(float x, float y, float z, float nx, float ny, float nz) :
			pos(DirectX::XMFLOAT3(x, y, z)),
			normal(DirectX::XMFLOAT3(nx, ny, nz)),
			uv(DirectX::XMFLOAT2(0.0f, 0.0f)) {};
	};

	// (dx12の管理クラス, 座標, テクスチャファイルパス)
	StaticMesh(std::weak_ptr<Actor>owner, Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, std::wstring texPath = L"");
	~StaticMesh();
	// 更新
	void Update()override;
	// 描画
	void Draw()override;

protected:
	ComPtr<ID3D12Resource> vbuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	ComPtr<ID3D12Resource> ibuffer_;
	D3D12_INDEX_BUFFER_VIEW ibv_;

	int indexNum_;

	std::unique_ptr<Material> material_;

	// アニメーションカウント
	unsigned int animCnt_;

	 // 頂点バッファとビューの作成
	void CreateVertexBufferAndView(std::vector<PrimVertex> vertices);
	// インデックスバッファとビューの作成
	void CreateIndexBufferAndView(std::vector<uint16_t> indices);

	// 各頂点の法線計算(頂点配列, インデックス配列)
	void CalNormalVertex(std::vector<PrimVertex>& vertices, const std::vector<uint16_t>& indices);
	// 三角形の表面法線の取得(頂点0, 頂点1, 頂点2)
	DirectX::XMVECTOR GetSurfaceNormal(const DirectX::XMVECTOR& v0, const DirectX::XMVECTOR& v1, const DirectX::XMVECTOR& v2);
	// 三角形の表面法線の取得(頂点配列, 中心)
	void CalNormalVertex2(std::vector<PrimVertex>& vertices, DirectX::XMFLOAT3 center);
};

