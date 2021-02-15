#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <array>
#include <DirectXMath.h>
#include "Utility/DirectXStruct.h"
#include <DirectXTex.h>
#include <unordered_map>
#include "Shader/Struct/UtilityShaderStruct.h"

class Dx12Wrapper;
class Material;

/// <summary>
/// �u�����h���[�h
/// </summary>
enum class BlendMode
{
	noblend,	// �m�[�u�����h
	alpha,		// ���u�����h
	add,		// ���Z�u�����h
	sub,		// ���Z�u�����h
	mula,		// ��Z�u�����h
	inv,		// ���]�u�����h
	max			// �I��ł͂����Ȃ�
};

/// <summary>
/// 2D�`��
/// </summary>
class SpriteDrawer
{
public:
	/// <summary>
	/// DX12�Ǘ��N���X
	/// </summary>
	/// <param name="dx12"></param>
	SpriteDrawer(Dx12Wrapper& dx12);
	~SpriteDrawer();

	/// <summary>
	/// �`�掞�Ɏg�p����}�e���A���̐ݒ�
	/// </summary>
	/// <param name="material">�}�e���A��</param>
	/// <returns>����</returns>
	bool SetMaterial(std::shared_ptr<Material> material);

	/// <summary>
	/// �摜�n���h�����w����W����`��
	/// </summary>
	/// <param name="x">�`�捶���W</param>
	/// <param name="y">�`�����W</param>
	/// <param name="graphHandle">�`��摜�n���h��</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool DrawGraph(const INT x, const INT y, const int graphHandle);

	/// <summary>
	/// �摜�n���h���𒆐S��]�`��
	/// �`����W�͒��S
	/// </summary>
	/// <param name="x">���SX���W</param>
	/// <param name="y">���SY���W</param>
	/// <param name="exRate">�g�嗦</param>
	/// <param name="angle">��](���W�A��)</param>
	/// <param name="graphHandle">�摜�n���h��</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool DrawRotaGraph(const INT x, const INT y, const float exRate, const float angle, const int graphHandle);
	
	/// <summary>
	/// ��]�`��
	/// �`����W�͒��S
	/// ��]�ʒu�𒆐S�Ƃ͕ʂɎw��\
	/// </summary>
	/// <param name="x">�`�撆�SX���W</param>
	/// <param name="y">�`�撆�SY���W</param>
	/// <param name="centerX">��]���SX���W</param>
	/// <param name="centerY">��]���SY���W</param>
	/// <param name="exRate">�g�嗦</param>
	/// <param name="angle">��](���W�A��)</param>
	/// <param name="graphHandle">�摜�n���h��</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool DrawRotaGraph2(const INT x, const INT y, const UINT centerX, const UINT centerY, const float exRate, const float angle, const int graphHandle);
	
	/// <summary>
	/// �摜�؂蔲���`��
	/// graphH�����`������`�悷��
	/// </summary>
	/// <param name="destX">�`�捶���W</param>
	/// <param name="destY">�`�����W</param>
	/// <param name="srcX">�؂蔲����`�̍����W</param>
	/// <param name="srcY">�؂蔲����`�̏���W</param>
	/// <param name="width">�؂蔲����`�̕�</param>
	/// <param name="height">�؂蔲����`�̍���</param>
	/// <param name="graphHandle">�摜�n���h��</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool DrawRectGraph(const INT destX, const INT destY, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);
	
	/// <summary>
	/// �摜�؂蔲����]�`��
	/// </summary>
	/// <param name="x">�`�撆�SX���W</param>
	/// <param name="y">�`�撆�SY���W</param>
	/// <param name="srcX">�؂蔲����`�̍����W</param>
	/// <param name="srcY">�؂蔲����`�̏���W</param>
	/// <param name="width">�؂蔲����`�̕�</param>
	/// <param name="height">�؂蔲����`�̍���</param>
	/// <param name="cx">��]���SX���W</param>
	/// <param name="cy">��]���SY���W</param>
	/// <param name="exRate">�g�嗦</param>
	/// <param name="angle">��](���W�A��)</param>
	/// <param name="graphHandle">�摜�n���h��</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool DrawRectRotaGraph2(const INT x, const INT y, const INT srcX, const INT srcY, const INT width, const INT height, const INT cx, const INT cy, const float exRate, const float angle, const int graphHandle);
	
	/// <summary>
	/// �摜�g��`��
	/// ����E���w��
	/// </summary>
	/// <param name="left">�`�捶���W</param>
	/// <param name="top">�`�����W</param>
	/// <param name="right">�`��E���W</param>
	/// <param name="buttom">�`�扺���W</param>
	/// <param name="graphHandle">�摜�n���h��</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool DrawExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const int graphHandle);
	
	/// <summary>
	/// �摜�؂蔲���g��`��
	/// </summary>
	/// <param name="left">�`�捶���W</param>
	/// <param name="top">�`�����W</param>
	/// <param name="right">�`��E���W</param>
	/// <param name="buttom">�`�扺���W</param>
	/// <param name="srcX">�؂蔲����`�̍����W</param>
	/// <param name="srcY">�؂蔲����`�̏���W</param>
	/// <param name="width">�؂蔲����`�̕�</param>
	/// <param name="height">�؂蔲����`�̍���</param>
	/// <param name="graphHandle">�摜�n���h��</param>
	/// <returns>�L���ȉ摜�n���h����</returns>
	bool DrawRectExtendGraph(const INT left, const INT top, const INT right, const INT buttom, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const int graphHandle);
	
	/// <summary>
	/// �`�施�߂����
	/// ���s����
	/// </summary>
	void End();

	/// <summary>
	/// �`�掞�̖��邳�ݒ�(0�`255)
	/// </summary>
	/// <param name="r">R����</param>
	/// <param name="g">G����</param>
	/// <param name="b">B����</param>
	void SetDrawBright(const INT r, const INT g, const INT b);

	/// <summary>
	/// �u�����h���[�h���[�h�̕ύX
	/// </summary>
	/// <param name="blendMode">�u�����h���[�h</param>
	/// <param name="value">�u�����h�l</param>
	void SetDrawBlendMode(const BlendMode blendMode, const INT value);

	/// <summary>
	/// �`���̃X�N���[�����Z�b�g����
	/// </summary>
	/// <param name="graphHandle">��ذ݉摜�n���h��</param>
	void SetDrawScreen(const int graphHandle);

private:
	SpriteDrawer(const SpriteDrawer&) = delete;
	SpriteDrawer& operator=(const SpriteDrawer&) = delete;

	Dx12Wrapper& dx12_;

	struct VerticesInf
	{
		DirectX::XMMATRIX posTans;
		DirectX::XMMATRIX uvTrans;
	};

	struct VertexResource
	{
		Resource resource;
		VerticesInf* mappedVertexInf = nullptr;
	};

	struct PixelInf
	{
		unsigned int texIndex = 0;
		DirectX::XMFLOAT3 bright = {};
		float alpha = 1.0f;
	};

	struct PixelInfResource
	{
		Resource resource;
		PixelInf* mappedPixelInf = nullptr;
	};

	struct DrawImage
	{
		VerticesInf verticesInf = {};
		PixelInf pixelInf = {};
	};

	Resource vertResource_;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	Resource indexResource_;
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	std::array<std::shared_ptr<Material>, static_cast<size_t>(BlendMode::max)> standeredBlendPipelineStates_;

	VertexResource verticesInfSB_;
	ComPtr<ID3D12DescriptorHeap> verticesInfHeap_ = nullptr;

	PixelInfResource pixelInfSB_;
	ComPtr<ID3D12DescriptorHeap> pixelInfHeap_ = nullptr;

	std::shared_ptr<Material> material_;
	DirectX::XMFLOAT3 drawBright_;
	float blendValue_;

	struct DrawGroup
	{
		std::shared_ptr<Material> material;
		int num = 1;
	};
	std::vector<DrawGroup> drawGroups_;
	std::vector<DrawImage> drawImages_;

	ComPtr<ID3DBlob> vertexShader_ = nullptr;
	ComPtr<ID3DBlob> pixelShader_ = nullptr;

	/// <summary>
	/// ����s�N�Z���V�F�[�_�[���Ƃ̃p�C�v���C���}�b�v
	/// </summary>
	std::unordered_map<std::wstring, std::shared_ptr<Material>> pipelineStateMap_;

	struct UtilityResource
	{
		Resource resource;
		Utility* mapped = nullptr;
	};

	// �V�F�[�_�[�ɑ���萔
	UtilityResource utility_;
	ComPtr<ID3D12DescriptorHeap> utilityHeap_ = nullptr;

	void CreatePiplineState();
	void CreateRootSignature();
	void CreateVertextBuffer();

	void CreateIndexBuffer();

	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const float exRate = 1.0f, const float angle = 0.0f);
	void SetPosTrans(DirectX::XMMATRIX& posTrans, const INT left, const INT top, const UINT width, const UINT height, const UINT centerX, const UINT centerY, const float exRate = 1.0f, const float angle = 0.0f);
	void SetUVTrans(DirectX::XMMATRIX& uvTrans, const UINT srcX, const UINT srcY, const UINT width, const UINT height, const DirectX::Image& img);
	void AddDrawImage(SpriteDrawer::DrawImage& drawImage);

	void ClearDrawData();

	void GetDefaultInputElementDesc(std::vector<D3D12_INPUT_ELEMENT_DESC>& ied);
	void GetDefaultPipelineStateDesc(const std::vector<D3D12_INPUT_ELEMENT_DESC>& ied, D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpsd);
};

