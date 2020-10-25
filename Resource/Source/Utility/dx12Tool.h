#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include "Tool.h"
#include "Utility/DirectXStruct.h"
#include <algorithm>

#define H_ASSERT(X) assert(SUCCEEDED(X));

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
	/// upload�p�o�b�t�@�̍쐬
	/// </summary>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="pBuffer">�i�[����o�b�t�@</param>
	/// <param name="resourceSize">���\�[�X�̃T�C�Y</param>
	/// <param name="ajustAligment">resourceSize�̃A���C�����g�𑵂��邩</param>
	void CreateUploadBuffer(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12Resource>& pBuffer, const UINT64& resourceSize, const bool ajustAligment = true)
	{
		auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resWidth = ajustAligment ? AlignmentValue(resourceSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT) : resourceSize;
		D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resWidth);

		H_ASSERT(dev->CreateCommittedResource(&heapPro, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(pBuffer.ReleaseAndGetAddressOf())));
	}

	/// <summary>
	/// upload�p�o�b�t�@�̍쐬
	/// </summary>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="resource">�i�[���郊�\�[�X</param>
	/// <param name="resourceSize">���\�[�X�̃T�C�Y</param>
	/// <param name="ajustAligment">resourceSize�̃A���C�����g�𑵂��邩</param>
	void CreateUploadResource(ID3D12Device* dev, Resource& resource, const UINT64& resourceSize, const bool ajustAligment = true)
	{
		resource.state = D3D12_RESOURCE_STATE_GENERIC_READ;
		CreateUploadBuffer(dev, resource.buffer, resourceSize, ajustAligment);
	}

	/// <summary>
	/// ���\�[�X�̃}�b�v���s��
	/// </summary>
	/// <typeparam name="T">�}�b�v���s���^</typeparam>
	/// <typeparam name="InputIterator">�R�s�[�͈͂�Iterator</typeparam>
	/// <param name="resource">�}�b�v���郊�\�[�X</param>
	/// <param name="begin">�R�s�[�͈͂̍ŏ�</param>
	/// <param name="end">�R�s�[�͈͂̍Ō�</param>
	/// <param name="ummap">�I������ummap���s����</param>
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
	/// DescriptorHeap�̍쐬
	/// </summary>
	/// <param name="dev">I3D12Device�̃|�C���^</param>
	/// <param name="pHeap">�i�[����q�[�v�̃|�C���^</param>
	/// <param name="numDescriptors">�f�X�N���v�^��</param>
	void CreateDescriptorHeap(ID3D12Device* dev, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& pHeap, UINT numDescriptors = 1)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NumDescriptors = numDescriptors;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		H_ASSERT(dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(pHeap.ReleaseAndGetAddressOf())));
	}

	/// <summary>
	/// �萔�o�b�t�@�r���[�̍쐬
	/// </summary>
	/// <param name="dev">>I3D12Device�̃|�C���^</param>
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
	/// �V�F�[�_�[�̃R���p�C�����s��
	/// </summary>
	/// <param name="shaderPath">�V�F�[�_�[�t�@�C���̃p�X</param>
	/// <param name="entoryPoint">�G���g���[�|�C���g��</param>
	/// <param name="shaderModel">�V�F�[�_�[���f��</param>
	/// <param name="shaderBlob">�R���p�C�������V�F�[�_�[���i�[����Bolb</param>
	/// <param name="errorBlob">�G���[�����i�[����Blob</param>
	void ShaderCompile(const LPCWSTR& shaderPath, const LPCSTR& entoryPoint, const LPCSTR& shaderModel, ComPtr<ID3DBlob>& shaderBlob)
	{
		ComPtr<ID3DBlob> erBlob = nullptr;

		if (FAILED(D3DCompileFromFile(shaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entoryPoint, shaderModel,
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, shaderBlob.ReleaseAndGetAddressOf(), erBlob.ReleaseAndGetAddressOf())))
		{
			OutputBolbString(erBlob.Get());
			assert(false);
		}
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