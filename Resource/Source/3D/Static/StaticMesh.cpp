#include "StaticMesh.h"
#include "System/Dx12Wrapper.h"
#include "d3dx12.h"
#include "System/TexLoader.h"
#include "Utility/dx12Tool.h"
#include "Utility/TextureStruct.h"
#include "Material/Material.h"
#include "3D/Actor.h"

using namespace DirectX;
using namespace std;

void StaticMesh::CreateVertexBufferAndView(std::vector<PrimVertex> vertices)
{
	auto& dev = dx12_.GetDevice();
	CreateBuffer(&dev, vbuffer_, D3D12_HEAP_TYPE_UPLOAD, sizeof(vertices[0]) * vertices.size());

	PrimVertex* verMap = nullptr;
	H_ASSERT(vbuffer_->Map(0, nullptr, (void**)&verMap));
	std::copy(vertices.begin(), vertices.end(), verMap);
	vbuffer_->Unmap(0, nullptr);

	vbv_ = {};
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());
	vbv_.StrideInBytes = sizeof(vertices[0]);
	vbv_.BufferLocation = vbuffer_->GetGPUVirtualAddress();
}

void StaticMesh::CreateIndexBufferAndView(std::vector<uint16_t> indices)
{
	auto& dev = dx12_.GetDevice();
	CreateBuffer(&dev, ibuffer_, D3D12_HEAP_TYPE_UPLOAD, sizeof(indices[0]) * indices.size());
	uint16_t* indMap = nullptr;
	H_ASSERT(ibuffer_->Map(0, nullptr, (void**)&indMap));
	copy(indices.begin(), indices.end(), indMap);
	ibuffer_->Unmap(0, nullptr);

	ibv_ = {};
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(indices[0]) * indices.size());
	ibv_.Format = DXGI_FORMAT_R16_UINT;
	ibv_.BufferLocation = ibuffer_->GetGPUVirtualAddress();

	indexNum_ = ibv_.SizeInBytes / sizeof(uint16_t);
}

void StaticMesh::CalNormalVertex(std::vector<PrimVertex>& vertices, const std::vector<uint16_t>& indices)
{
	for (int j = 0; j < vertices.size(); j++)
	{
		XMVECTOR normal = {};
		for (int k = 0; k < indices.size(); k+=3)
		{
			if (j == indices[k + 0]
			||  j == indices[k + 1]
			||  j == indices[k + 2])
			{
				auto vo = XMLoadFloat3(&vertices[indices[k + 0]].pos);
				auto v1 = XMLoadFloat3(&vertices[indices[k + 1]].pos);
				auto v2 = XMLoadFloat3(&vertices[indices[k + 2]].pos);
				normal += GetSurfaceNormal(vo, v1, v2);
			}
		}
		XMStoreFloat3(&vertices[j].normal, XMVector3Normalize(normal));
	}
}

XMVECTOR StaticMesh::GetSurfaceNormal(const XMVECTOR & v0, const XMVECTOR & v1, const XMVECTOR & v2)
{
	auto vv1 = v1 - v0;
	auto vv2 = v2 - v1;
	auto norm = XMVector3Cross(vv1, vv2);
	return XMVector3Normalize(norm);
}

void StaticMesh::CalNormalVertex2(std::vector<PrimVertex>& vertices, XMFLOAT3 center)
{
	for (auto& vert : vertices)
	{
		auto vertPos = XMLoadFloat3(&vert.pos);
		auto centerPos = XMLoadFloat3(&center);
		XMStoreFloat3(&vert.normal, XMVector4Normalize(vertPos - centerPos));
	}
}

StaticMesh::StaticMesh(std::weak_ptr<Actor>owner,
	Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, std::wstring texPath):
	Mesh(dx12, owner, Mesh::Type::static_mesh)
{
	animCnt_ = 0;
	vbv_ = {};
	ibv_ = {};
	indexNum_ = 0;
	auto actor = GetOwner().lock();
	auto trans = actor->GetTransform();
	trans.pos = pos;
	actor->SetTransform(trans);
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::Update()
{
}

void StaticMesh::Draw()
{
	// 座標行列用デスクリプタヒープのセット
	auto& cmdList = dx12_.GetCommand().CommandList();
	material_->SetEachDescriptorHeap(cmdList);

	// 座標のセット
	GetOwner().lock()->SetTransformHeap(7);

	// インデックスバッファのセット
	cmdList.IASetIndexBuffer(&ibv_);
	// 頂点バッファビューの設定
	cmdList.IASetVertexBuffers(0, 1, &vbv_);

	cmdList.DrawIndexedInstanced(indexNum_, 1, 0, 0, 0);
}