#pragma once
#include "3D/Component.h"
class Actor;

/// <summary>
/// メッシュ基底クラス
/// </summary>
class Mesh
	:public Component
{
public:
	enum class Type
	{
		// ボーン無しメッシュ
		static_mesh,
		// ボーン有メッシュ
		skeletal_mesh,

		max
	};

	/// <param name="type">メッシュタイプ</param>
	/// <param name="owner">所有者</param>
	Mesh(const Mesh::Type type, std::shared_ptr<Actor>owner);
	~Mesh()=default;

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

private:
	const Mesh::Type type_;
};

