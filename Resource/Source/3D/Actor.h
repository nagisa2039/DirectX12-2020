#pragma once
#include "Utility/ComPtr.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <memory>
#include <vector>

class Component;

// トランスフォーム情報
struct Transform
{
	// 座標
	DirectX::XMFLOAT3 pos		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// 回転
	DirectX::XMFLOAT3 rotate	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// 拡大
	DirectX::XMFLOAT3 scale		= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	DirectX::XMMATRIX GetMatrix()const;
	DirectX::XMMATRIX GetRotateMatrix()const;
	DirectX::XMFLOAT3 GetForwerd()const;
};

/// <summary>
/// 3Dオブジェクト基底クラス
/// </summary>
class Actor
	: public std::enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// トランスフォームの取得
	/// </summary>
	const Transform& GetTransform()const;

	/// <summary>
	/// トランスフォームの設定
	/// </summary>
	void SetTransform(const Transform& transform);

	/// <summary>
	/// Transformの更新
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// CommandListにtrans情報を追加(セット)する
	/// </summary>
	/// <param name="rootParamatorIndex">rootParamatorのインデックス</param>
	void SetTransformHeap(const UINT rootParamatorIndex);

	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <param name="component">対象のコンポーネント</param>
	void AddComponent(std::shared_ptr<Component> component);

protected:
	/// <summary>
	/// トランスフォーム情報を格納したヒープの取得
	/// </summary>
	const ComPtr<ID3D12DescriptorHeap>& GetTransformHeap()const;

private:
	Transform trans_;
	DirectX::XMMATRIX* mappedTrans_;

	// 座標行列用定数バッファ
	ComPtr<ID3D12Resource> transCB_ = nullptr;
	// transCBを入れるヒープ
	ComPtr<ID3D12DescriptorHeap> transHeap_ = nullptr;

	std::vector<std::shared_ptr<Component>> components_;
};

