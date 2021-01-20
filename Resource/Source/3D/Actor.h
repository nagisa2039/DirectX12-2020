#pragma once
#include "Utility/ComPtr.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <memory>
#include <vector>
#include <string>

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

	/// <summary>
	/// Transform行列の取得
	/// </summary>
	DirectX::XMMATRIX GetMatrix()const;

	/// <summary>
	/// 回転行列の取得
	/// </summary>
	DirectX::XMMATRIX GetRotateMatrix()const;

	/// <summary>
	/// 前方向ベクトルの取得
	/// </summary>
	DirectX::XMVECTOR GetForwerd()const;

	/// <summary>
	/// 上方向ベクトルの取得
	/// </summary>
	DirectX::XMVECTOR GetUp()const;

	/// <summary>
	/// 右方向ベクトルの取得
	/// </summary>
	DirectX::XMVECTOR GetRight()const;
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
	/// 名前の取得
	/// </summary>
	const std::string& GetName()const;

	/// <summary>
	/// 名前の設定
	/// </summary>
	/// <param name="n">設定する名前</param>
	void SetName(const std::string& n);

	/// <summary>
	/// トランスフォームの取得
	/// </summary>
	Transform& GetTransform();

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
	/// <param name="compute">Compute用か</param>
	void SetTransformHeap(const UINT rootParamatorIndex, const bool compute = false);

	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <param name="component">対象のコンポーネント</param>
	void AddComponent(std::shared_ptr<Component> component);

	/// <summary>
	/// ImGuiの描画
	/// </summary>
	/// <param name="num">Node識別番号</param>
	virtual void DrawImGui(const int num);

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

	std::string name_;
};

