#pragma once
#include "3D/Component.h"

class Actor;
class Dx12Wrapper;

/// <summary>
/// メッシュ基底クラス
/// </summary>
class Mesh
	:public Component
{
public:
	// メッシュの種類
	enum class Type
	{
		// ボーン無しメッシュ
		static_mesh,
		// ボーン有メッシュ
		skeletal_mesh,

		max
	};

	/// <param name="dx12">DirectX12管理クラス</param>
	/// <param name="owner">所有者</param>
	/// <param name="type">メッシュタイプ</param>
	/// <param name=""></param>
	Mesh(Dx12Wrapper& dx12, std::weak_ptr<Actor>owner, const Mesh::Type type);
	virtual ~Mesh();

	/// <summary>
	/// 初期化
	/// コンストラクタでできない(自身が作られないと出来ない)
	/// 処理を行う
	/// </summary>
	virtual void Init()override;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update()=0;

	/// <summary>
	/// 通常描画
	/// </summary>
	virtual void Draw()=0;

	/// <summary>
	/// メッシュタイプの取得
	/// </summary>
	Mesh::Type GetMeshType()const;

protected:
	// DirectX12管理クラス
	Dx12Wrapper& dx12_;

private:
	const Mesh::Type type_;
};

