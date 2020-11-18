#include "PrimitiveMesh.h"
#include "System/Dx12Wrapper.h"
#include "d3dx12.h"
#include "System/TexLoader.h"
#include "Utility/dx12Tool.h"
#include "Utility/TextureStruct.h"
#include "Material/Material.h"

using namespace DirectX;
using namespace std;

void PrimitiveMesh::CreateVertexBufferAndView(std::vector<PrimVertex> vertices)
{
	auto& dev = dx12_.GetDevice();
	CreateUploadBuffer(&dev, vbuffer_, sizeof(vertices[0]) * vertices.size(), true);

	PrimVertex* verMap = nullptr;
	H_ASSERT(vbuffer_->Map(0, nullptr, (void**)&verMap));
	std::copy(vertices.begin(), vertices.end(), verMap);
	vbuffer_->Unmap(0, nullptr);

	vbv_ = {};
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());
	vbv_.StrideInBytes = sizeof(vertices[0]);
	vbv_.BufferLocation = vbuffer_->GetGPUVirtualAddress();
}

void PrimitiveMesh::CreateIndexBufferAndView(std::vector<uint16_t> indices)
{
	auto& dev = dx12_.GetDevice();
	CreateUploadBuffer(&dev, ibuffer_, sizeof(indices[0]) * indices.size(), true);
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

void PrimitiveMesh::CreateTransBuffer()
{
	// 座標の定数バッファの作成
	CreateUploadBuffer(&dx12_.GetDevice(), transCB_, sizeof(*mappedTrans_));
	transCB_->Map(0, nullptr, (void**)&mappedTrans_);

	*mappedTrans_ =
		XMMatrixRotationRollPitchYaw(rotate_.x, rotate_.y, rotate_.z)
		*XMMatrixTranslation(pos_.x, pos_.y, pos_.z);

	// 座標のヒープ作成
	CreateDescriptorHeap(&dx12_.GetDevice(), worldHeap_);

	// 定数バッファビューの作成
	CreateConstantBufferView(&dx12_.GetDevice(), transCB_, worldHeap_->GetCPUDescriptorHandleForHeapStart());
}

void PrimitiveMesh::CalNormalVertex(std::vector<PrimVertex>& vertices, const std::vector<uint16_t>& indices)
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

XMVECTOR PrimitiveMesh::GetSurfaceNormal(const XMVECTOR & v0, const XMVECTOR & v1, const XMVECTOR & v2)
{
	auto vv1 = v1 - v0;
	auto vv2 = v2 - v1;
	auto norm = XMVector3Cross(vv1, vv2);
	return XMVector3Normalize(norm);
}

void PrimitiveMesh::CalNormalVertex2(std::vector<PrimVertex>& vertices, XMFLOAT3 center)
{
	for (auto& vert : vertices)
	{
		auto vertPos = XMLoadFloat3(&vert.pos);
		auto centerPos = XMLoadFloat3(&center);
		XMStoreFloat3(&vert.normal, XMVector4Normalize(vertPos - centerPos));
	}
}

PrimitiveMesh::PrimitiveMesh(Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, std::wstring texPath):dx12_(dx12), pos_(pos)
{
	animCnt_ = 0;
}

PrimitiveMesh::~PrimitiveMesh()
{
}

void PrimitiveMesh::Update()
{
	animCnt_++;

	*mappedTrans_ =
		XMMatrixRotationRollPitchYaw(rotate_.x * XM_PI / 180.0f, rotate_.y * XM_PI / 180.0f, rotate_.z * XM_PI / 180.0f)
		*XMMatrixTranslation(pos_.x, pos_.y, pos_.z);
}

void PrimitiveMesh::Draw()
{
	// 座標行列用デスクリプタヒープのセット
	auto& cmdList = dx12_.GetCommand().CommandList();
	material_->SetEachDescriptorHeap(cmdList);

	cmdList.SetDescriptorHeaps(1, worldHeap_.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(7, worldHeap_->GetGPUDescriptorHandleForHeapStart());

	// インデックスバッファのセット
	cmdList.IASetIndexBuffer(&ibv_);
	// 頂点バッファビューの設定
	cmdList.IASetVertexBuffers(0, 1, &vbv_);

	cmdList.DrawIndexedInstanced(indexNum_, 1, 0, 0, 0);
}