#pragma once
#include <memory>
#include <vector>
#include <array>
#include <d3d12.h>
#include <string>
#include "Utility/ComPtr.h"

class Renderer;
class Dx12Wrapper;
class ModelEndRendering;
class Mesh;
class CameraObject;

/// <summary>
/// 3D���b�V���̕`��Ǘ��N���X
/// </summary>
class RendererManager
{
public:
	// �}���`�p�X�����_�����O�̎��
	enum class RenderTargetType
	{
		color,
		normal,
		bright,
		max
	};

	/// <param name="dx12">DirectX12�Ǘ��N���X</param>
	RendererManager(Dx12Wrapper& dx12);
	~RendererManager();

	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// ���b�V���̒ǉ�
	/// </summary>
	/// <param name="mesh"></param>
	void AddMesh(Mesh* mesh);

	/// <summary>
	/// ���b�V���̍폜
	/// </summary>
	/// <param name="mesh"></param>
	void RemoveMesh(Mesh* mesh);

private:
	Dx12Wrapper& dx12_;

	struct MeshRender
	{
		std::shared_ptr<Renderer> renderer;
		std::vector<Mesh*> meshs_;
	};
	std::vector<MeshRender> meshRenderers_;

	int cameraScreenH_;
	int shrinkScreenH_;

	ComPtr<ID3D12DescriptorHeap> renderTargetHeap_;
	std::array<int, static_cast<uint64_t>(RenderTargetType::max)> rendetTargetHandles_;

	std::shared_ptr<ModelEndRendering> modelEndrendering_;
	std::shared_ptr<CameraObject> camera_;

	void CreateRenderTargetHeap();
};