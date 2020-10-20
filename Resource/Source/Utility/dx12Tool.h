#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include "Tool.h"
#include "Constant.h"

namespace
{
	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	/// <param name="dev">I3D12Deviceのポインタ</param>
	/// <param name="pBuffer">格納するバッファ</param>
	/// <param name="resourceSize">リソースのサイズ</param>
	void CreateConstantBuffer(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& pBuffer, UINT resourceSize)
	{
		auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resWidth = AlignmentValue(resourceSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resWidth);

		H_ASSERT(dev->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(pBuffer.ReleaseAndGetAddressOf())));
	}

	/// <summary>
	/// DescriptorHeapの作成
	/// </summary>
	/// <param name="dev">I3D12Deviceのポインタ</param>
	/// <param name="pHeap">格納するヒープのポインタ</param>
	/// <param name="numDescriptors">デスクリプタ数</param>
	void CreateDescriptorHeap(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pHeap, UINT numDescriptors = 1)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NumDescriptors = numDescriptors;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		H_ASSERT(dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(pHeap.ReleaseAndGetAddressOf())));
	}

	/// <summary>
	/// 定数バッファビューの作成
	/// </summary>
	/// <param name="dev">>I3D12Deviceのポインタ</param>
	/// <param name="buff">ビューを作りたいバッファ</param>
	/// <param name="handle">バッファを格納するデスクリプタのハンドル</param>
	void CreateConstantBufferView(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
		viewDesc.BufferLocation = buff->GetGPUVirtualAddress();
		viewDesc.SizeInBytes = static_cast<UINT>(buff->GetDesc().Width);
		dev->CreateConstantBufferView(&viewDesc, handle);
	}
	
	/// <summary>
	/// シェーダーのコンパイルを行う
	/// </summary>
	/// <param name="shaderPath">シェーダーファイルのパス</param>
	/// <param name="entoryPoint">エントリーポイント名</param>
	/// <param name="shaderModel">シェーダーモデル</param>
	/// <param name="shaderBlob">コンパイルしたシェーダーを格納するBolb</param>
	/// <param name="errorBlob">エラー情報を格納するBlob</param>
	void ShaderCompile(const LPCWSTR& shaderPath, const LPCSTR& entoryPoint, const LPCSTR& shaderModel, ComPtr<ID3DBlob>& shaderBlob, ComPtr<ID3DBlob>& errorBlob)
	{
		H_ASSERT(D3DCompileFromFile(shaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entoryPoint, shaderModel,
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, shaderBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf()));
	}
}