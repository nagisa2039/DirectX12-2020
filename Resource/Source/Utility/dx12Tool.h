#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include "Tool.h"
#include "Utility/DirectXStruct.h"
#include "System/Command.h"
#include <algorithm>


namespace
{
	/// <summary>
	/// ID3DBlob�̒��g���f�o�b�O�o�͂ɕ\��
	/// </summary>
	/// <param name="blob">ID3DBlob�̃|�C���^</param>
	void OutputBolbString(ID3DBlob* blob)
	{
		if (!blob)return;

		std::string str;
		str.resize(blob->GetBufferSize());
		std::copy_n((char*)blob->GetBufferPointer(), blob->GetBufferSize(), &str[0]);
		OutputDebugStringA(str.data());
	}

	/// <summary>
	/// �o�b�t�@�Ƀo���A���s��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="buff">�Ώۂ̃o�b�t�@</param>
	/// <param name="before">���܂ł̏��</param>
	/// <param name="after">�J�ڂ�����</param>
	void ResourceBarrier(ID3D12GraphicsCommandList* cmdList, Microsoft::WRL::ComPtr<ID3D12Resource>& buff,
		const D3D12_RESOURCE_STATES before, const D3D12_RESOURCE_STATES after)
	{
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buff.Get(), before, after);
		cmdList->ResourceBarrier(1, &barrier);
	}

	/// <summary>
	/// �o�b�t�@�̍쐬
	/// </summary>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="pBuffer">�i�[����o�b�t�@</param>
	/// <param name="heapType">�q�[�v�̎�ނ��w��</param>
	/// <param name="resourceSize">���\�[�X�̃T�C�Y</param>
	/// <param name="ajustAligment">resourceSize�̃A���C�����g�𑵂��邩</param>
	void CreateBuffer(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& pBuffer, 
		const D3D12_HEAP_TYPE heapType, const UINT64& resourceSize, 
		const D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_GENERIC_READ, const bool ajustAligment = true)
	{
		auto heapPro = CD3DX12_HEAP_PROPERTIES(heapType);
		auto resWidth = ajustAligment ? AlignmentValue(resourceSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) : resourceSize;
		D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resWidth);

		H_ASSERT(dev->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &resDesc,
			state, nullptr, IID_PPV_ARGS(pBuffer.ReleaseAndGetAddressOf())));
	}

	/// <summary>
	/// ���\�[�X�̃}�b�v���s��
	/// </summary>
	/// <typeparam name="T">�}�b�v���s���^</typeparam>
	/// <param name="mapped"></param>
	/// <param name="resource"></param>
	template<typename T>
	void Map(T* mapped, Resource& resource)
	{
		H_ASSERT(resource.buffer->Map(0, nullptr, (void**)&mapped));
	}

	/// <summary>
	/// ���\�[�X�̃}�b�v�ƃR�s�[���s��
	/// </summary>
	/// <typeparam name="T">�}�b�v���s���^</typeparam>
	/// <typeparam name="InputIterator">�R�s�[�͈͂�Iterator</typeparam>
	/// <param name="mapped">�}�b�v�̑Ώ�</param>
	/// <param name="resource">�}�b�v���郊�\�[�X</param>
	/// <param name="begin">�R�s�[�͈͂̍ŏ�</param>
	/// <param name="end">�R�s�[�͈͂̍Ō�</param>
	/// <param name="ummap">�I������ummap���s����</param>
	template<typename T, typename InputIterator>
	void MapAndCopy(T* mapped, Resource& resource, InputIterator begin, InputIterator end, bool ummap = true)
	{
		Map(mapped, resource);
		H_ASSERT(resource.buffer->Map(0, nullptr, (void**)&mapped));
		std::copy(begin, end, mapped);

		if (ummap)
		{
			resource.buffer->Unmap(0, nullptr);
		}
	}


	/// <summary>
	/// DescriptorHeap�̍쐬
	/// </summary>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="pHeap">�i�[����q�[�v�̃|�C���^</param>
	/// <param name="numDescriptors">�f�X�N���v�^��</param>
	void CreateDescriptorHeap(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pHeap, 
		D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, UINT numDescriptors = 1)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NumDescriptors = numDescriptors;
		heapDesc.Type = heapType;

		H_ASSERT(dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(pHeap.ReleaseAndGetAddressOf())));
	}

	/// <summary>
	/// �萔�o�b�t�@�r���[�̍쐬
	/// </summary>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="buff">�r���[����肽���o�b�t�@</param>
	/// <param name="handle">�o�b�t�@���i�[����f�X�N���v�^�̃n���h��</param>
	void CreateConstantBufferView(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
		viewDesc.BufferLocation = buff->GetGPUVirtualAddress();
		viewDesc.SizeInBytes = static_cast<UINT>(buff->GetDesc().Width);
		dev->CreateConstantBufferView(&viewDesc, handle);
	}

	/// <summary>
	/// �V�F�[�_�[���\�[�X�r���[�̍쐬
	/// </summary>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="buff">�r���[����肽���o�b�t�@</param>
	/// <param name="handle">�o�b�t�@���i�[����f�X�N���v�^�̃n���h��</param>
	/// <param name="structSize">�o�b�t�@�̃G�������g(struct)�T�C�Y</param>
	/// <param name="structNum">�o�b�t�@�̃G�������g(struct)��</param>
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
	/// StructuredBuffer�̍쐬
	/// </summary>
	/// <typeparam name="T">Struct�̌^</typeparam>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="buff">�i�[����o�b�t�@</param>
	/// <param name="heap">�i�[����q�[�v</param>
	/// <param name="elements">����\���̔z��</param>
	template<class T>
	void CreateStructuredBufferAndHeap(ID3D12Device* dev, T& mapped, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, 
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& heap, const UINT elementSize, const UINT elementNum)
	{
		assert(elementSize > 0 && elementNum > 0);

		CreateBuffer(dev, buff, D3D12_HEAP_TYPE_UPLOAD, static_cast<uint64_t>(elementSize) * elementNum);
		H_ASSERT(buff->Map(0, nullptr, (void**)&mapped));
		// �f�X�N���v�^�q�[�v�̍쐬
		CreateDescriptorHeap(dev, heap);

		// �萔�o�b�t�@�r���[�̍쐬
		auto handle = heap->GetCPUDescriptorHandleForHeapStart();
		CreateShaderResourceBufferView(dev, buff, handle, elementSize, elementNum);
	}

	/// <summary>
	/// StructuredBuffer�̍쐬
	/// </summary>
	/// <typeparam name="T">Struct�̌^</typeparam>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="buff">�i�[����o�b�t�@</param>
	/// <param name="heap">�i�[����q�[�v</param>
	/// <param name="elements">����\���̔z��</param>
	template<class T>
	void CreateStructuredBuffer(ID3D12Device* dev, Command& cmd, Microsoft::WRL::ComPtr<ID3D12Resource>& buff, 
		D3D12_CPU_DESCRIPTOR_HANDLE& handle, const std::vector<T>& elements, T*& mapped, const bool unmap)
	{
		const UINT elementNum = Uint32(elements.size());
		assert(elementNum > 0);
		const UINT elementSize = sizeof(T);
		auto elementBytes = Uint64(elementSize) * elementNum;
		mapped = nullptr;

		if (unmap)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> source;
			T* sourceMap = nullptr;
			auto& cmdList = cmd.CommandList();
			CreateBuffer(dev, source, D3D12_HEAP_TYPE_UPLOAD,  elementBytes, D3D12_RESOURCE_STATE_GENERIC_READ, false);
			H_ASSERT(source->Map(0, nullptr, (void**)&sourceMap));
			std::copy(elements.begin(), elements.end(), sourceMap);
			source->Unmap(0, nullptr);

			CreateBuffer(dev, buff, D3D12_HEAP_TYPE_DEFAULT, elementBytes, D3D12_RESOURCE_STATE_COPY_DEST,	 false);
			cmdList.CopyBufferRegion(buff.Get(), 0, source.Get(), 0, elementBytes);
			ResourceBarrier(&cmdList, buff, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
			cmd.Execute();
		}
		else
		{
			CreateBuffer(dev, buff, D3D12_HEAP_TYPE_UPLOAD, elementBytes, D3D12_RESOURCE_STATE_GENERIC_READ,false);
			H_ASSERT(buff->Map(0, nullptr, (void**)&mapped));
			std::copy(elements.begin(), elements.end(), mapped);
		}

		// �萔�o�b�t�@�r���[�̍쐬
		CreateShaderResourceBufferView(dev, buff, handle, elementSize, elementNum);
	}

	/// <summary>
	/// Shader�̒�`����RootSignature���쐬
	/// </summary>
	/// <param name="dev">�f�o�C�X</param>
	/// <param name="rootSignature">���[�g�V�O�l�`���̊i�[��</param>
	/// <param name="shader">��`��ǂݍ��ރV�F�[�_�[</param>
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