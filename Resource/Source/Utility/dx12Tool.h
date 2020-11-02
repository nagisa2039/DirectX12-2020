#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include "Tool.h"
#include "Utility/DirectXStruct.h"
#include <algorithm>


namespace
{
	/// <summary>
	/// ID3DBlobの中身をデバッグ出力に表示
	/// </summary>
	/// <param name="blob">ID3DBlobのポインタ</param>
	void OutputBolbString(ID3DBlob* blob)
	{
		if (!blob)return;

		std::string str;
		str.resize(blob->GetBufferSize());
		std::copy_n((char*)blob->GetBufferPointer(), blob->GetBufferSize(), &str[0]);
		OutputDebugStringA(str.data());
	}

	/// <summary>
	/// upload用バッファの作成
	/// </summary>
	/// <param name="dev">I3D12Deviceのポインタ</param>
	/// <param name="pBuffer">格納するバッファ</param>
	/// <param name="resourceSize">リソースのサイズ</param>
	/// <param name="ajustAligment">resourceSizeのアライメントを揃えるか</param>
	void CreateUploadBuffer(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& pBuffer, const UINT64& resourceSize, const bool ajustAligment = true)
	{
		auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resWidth = ajustAligment ? AlignmentValue(resourceSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) : resourceSize;
		D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resWidth);

		H_ASSERT(dev->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(pBuffer.ReleaseAndGetAddressOf())));
	}

	/// <summary>
	/// upload用バッファの作成
	/// </summary>
	/// <param name="dev">I3D12Deviceのポインタ</param>
	/// <param name="resource">格納するリソース</param>
	/// <param name="resourceSize">リソースのサイズ</param>
	/// <param name="ajustAligment">resourceSizeのアライメントを揃えるか</param>
	void CreateUploadResource(ID3D12Device* dev, Resource& resource, const UINT64& resourceSize, const bool ajustAligment = true)
	{
		resource.state = D3D12_RESOURCE_STATE_GENERIC_READ;
		CreateUploadBuffer(dev, resource.buffer, resourceSize, ajustAligment);
	}

	/// <summary>
	/// リソースのマップを行う
	/// </summary>
	/// <typeparam name="T">マップを行う型</typeparam>
	/// <typeparam name="InputIterator">コピー範囲のIterator</typeparam>
	/// <param name="resource">マップするリソース</param>
	/// <param name="begin">コピー範囲の最初</param>
	/// <param name="end">コピー範囲の最後</param>
	/// <param name="ummap">終了時にummapを行うか</param>
	template<typename T, typename InputIterator>
	void Map(T* mapped, Resource& resource, InputIterator begin, InputIterator end, bool ummap = true)
	{
		H_ASSERT(resource.buffer->Map(0, nullptr, (void**)&mapped));
		std::copy(begin, end, mapped);

		if (ummap)
		{
			resource.buffer->Unmap(0, nullptr);
		}
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
	/// <param name="dev">I3D12Deviceのポインタ</param>
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
	/// シェーダーリソースビューの作成
	/// </summary>
	/// <param name="dev">I3D12Deviceのポインタ</param>
	/// <param name="buff">ビューを作りたいバッファ</param>
	/// <param name="handle">バッファを格納するデスクリプタのハンドル</param>
	/// <param name="structSize">バッファのエレメント(struct)サイズ</param>
	/// <param name="structNum">バッファのエレメント(struct)数</param>
	void CreateShaderResourceBufferView(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, 
		const D3D12_CPU_DESCRIPTOR_HANDLE& handle, const UINT structSize, const UINT structNum)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
		viewDesc.Buffer.NumElements = structNum;
		viewDesc.Buffer.StructureByteStride = structSize;
		viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		viewDesc.Format = buff->GetDesc().Format;
		dev->CreateShaderResourceView(buff.Get(), &viewDesc, handle);
	}
	
	/// <summary>
	/// StructuredBufferの作成
	/// </summary>
	/// <typeparam name="T">Structの型</typeparam>
	/// <param name="dev">I3D12Deviceのポインタ</param>
	/// <param name="buff">格納するバッファ</param>
	/// <param name="heap">格納するヒープ</param>
	/// <param name="elements">送る構造体配列</param>
	template<class T>
	void CreateStructuredBuffer(ID3D12Device* dev, T& mapped, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, 
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap, const UINT elementSize, const UINT elementNum)
	{
		assert(elementSize > 0 && elementNum > 0);

		CreateUploadBuffer(dev, buff, elementSize * elementNum);
		H_ASSERT(buff->Map(0, nullptr, (void**)&mapped));
		// デスクリプタヒープの作成
		CreateDescriptorHeap(dev, heap);

		// 定数バッファビューの作成
		auto handle = heap->GetCPUDescriptorHandleForHeapStart();
		CreateShaderResourceBufferView(dev, buff, handle, elementSize, elementNum);
	}

	/// <summary>
	/// StructuredBufferの作成
	/// </summary>
	/// <typeparam name="T">Structの型</typeparam>
	/// <param name="dev">I3D12Deviceのポインタ</param>
	/// <param name="buff">格納するバッファ</param>
	/// <param name="heap">格納するヒープ</param>
	/// <param name="elements">送る構造体配列</param>
	template<class T>
	void CreateStructuredBuffer(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, 
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap, const std::vector<T>& elements)
	{
		const UINT elementNum = Uint32(elements.size());
		assert(elementNum > 0);
		const UINT elementSize = sizeof(elements[0]);

		CreateUploadBuffer(dev, buff, Uint64(elementSize) * elementNum, false);
		T* mapped = nullptr;
		H_ASSERT(buff->Map(0, nullptr, (void**)&mapped));
		std::copy(elements.begin(), elements.end(), mapped);
		buff->Unmap(0, nullptr);
		// デスクリプタヒープの作成
		CreateDescriptorHeap(dev, heap);

		// 定数バッファビューの作成
		auto handle = heap->GetCPUDescriptorHandleForHeapStart();
		CreateShaderResourceBufferView(dev, buff, handle, elementSize, elementNum);
	}

	/// <summary>
	/// Shaderの定義からRootSignatureを作成
	/// </summary>
	/// <param name="dev">デバイス</param>
	/// <param name="rootSignature">ルートシグネチャの格納先</param>
	/// <param name="shader">定義を読み込むシェーダー</param>
	void CreateRootSignatureFromShader(ID3D12Device* dev, ComPtr<ID3D12RootSignature>& rootSignature, const ComPtr<ID3DBlob>& shader)
	{ 
		ComPtr<ID3DBlob> signature = nullptr;

		H_ASSERT(D3DGetBlobPart(shader->GetBufferPointer(), shader->GetBufferSize(),
			D3D_BLOB_ROOT_SIGNATURE, 0, signature.GetAddressOf()));

		H_ASSERT(dev->CreateRootSignature(0,
			signature->GetBufferPointer(), signature->GetBufferSize(),
			IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf())));
	}
}