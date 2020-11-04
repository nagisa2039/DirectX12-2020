#include "PrimitiveMesh.h"
#include "System/Dx12Wrapper.h"
#include "d3dx12.h"
#include "System/TexLoader.h"
#include "Utility/dx12Tool.h"
#include "Utility/TextureStruct.h"

using namespace DirectX;
using namespace std;

void PrimitiveMesh::CreateVertexBufferAndView(std::vector<PrimVertex> vertices)
{
	auto& dev = _dx12.GetDevice();
	CreateUploadBuffer(&dev, _vbuffer, sizeof(vertices[0]) * vertices.size(), true);

	PrimVertex* verMap = nullptr;
	H_ASSERT(_vbuffer->Map(0, nullptr, (void**)&verMap));
	std::copy(vertices.begin(), vertices.end(), verMap);
	_vbuffer->Unmap(0, nullptr);

	_vbv = {};
	_vbv.SizeInBytes = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());
	_vbv.StrideInBytes = sizeof(vertices[0]);
	_vbv.BufferLocation = _vbuffer->GetGPUVirtualAddress();
}

void PrimitiveMesh::CreateIndexBufferAndView(std::vector<uint16_t> indices)
{
	auto& dev = _dx12.GetDevice();
	CreateUploadBuffer(&dev, _ibuffer, sizeof(indices[0]) * indices.size(), true);
	uint16_t* indMap = nullptr;
	H_ASSERT(_ibuffer->Map(0, nullptr, (void**)&indMap));
	copy(indices.begin(), indices.end(), indMap);
	_ibuffer->Unmap(0, nullptr);

	_ibv = {};
	_ibv.SizeInBytes = static_cast<UINT>(sizeof(indices[0]) * indices.size());
	_ibv.Format = DXGI_FORMAT_R16_UINT;
	_ibv.BufferLocation = _ibuffer->GetGPUVirtualAddress();

	_indexNum = _ibv.SizeInBytes / sizeof(uint16_t);
}

void PrimitiveMesh::CreateTransBuffer()
{
	// 座標の定数バッファの作成
	CreateUploadBuffer(&_dx12.GetDevice(), _transCB, sizeof(*_mappedTrans));
	_transCB->Map(0, nullptr, (void**)&_mappedTrans);

	*_mappedTrans =
		XMMatrixRotationRollPitchYaw(_rotate.x, _rotate.y, _rotate.z)
		*XMMatrixTranslation(_pos.x, _pos.y, _pos.z);

	// 座標のヒープ作成
	CreateDescriptorHeap(&_dx12.GetDevice(), _worldHeap, 1);

	// 定数バッファビューの作成
	CreateConstantBufferView(&_dx12.GetDevice(), _transCB, _worldHeap->GetCPUDescriptorHandleForHeapStart());
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

PrimitiveMesh::PrimitiveMesh(Dx12Wrapper& dx12, const DirectX::XMFLOAT3& pos, std::wstring texPath):_dx12(dx12), _pos(pos)
{
	_animCnt = 0;
}

PrimitiveMesh::~PrimitiveMesh()
{
}

void PrimitiveMesh::Update()
{
	_animCnt++;

	*_mappedTrans =
		XMMatrixRotationRollPitchYaw(_rotate.x * XM_PI / 180.0f, _rotate.y * XM_PI / 180.0f, _rotate.z * XM_PI / 180.0f)
		*XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}

void PrimitiveMesh::Draw()
{
	// 座標行列用デスクリプタヒープのセット
	auto& cmdList = _dx12.GetCommand().CommandList();
	cmdList.SetDescriptorHeaps(1, _worldHeap.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(1, _worldHeap->GetGPUDescriptorHandleForHeapStart());

	// テクスチャ
	cmdList.SetDescriptorHeaps(1, _texHeap.GetAddressOf());
	cmdList.SetGraphicsRootDescriptorTable(3, _texHeap->GetGPUDescriptorHandleForHeapStart());

	// インデックスバッファのセット
	cmdList.IASetIndexBuffer(&_ibv);
	// 頂点バッファビューの設定
	cmdList.IASetVertexBuffers(0, 1, &_vbv);

	cmdList.DrawIndexedInstanced(_indexNum, 1, 0, 0, 0);
}