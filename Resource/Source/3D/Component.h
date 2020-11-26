#pragma once
#include <memory>

class Actor;

/// <summary>
/// アクターに所有させるクラスの基底クラス
/// </summary>
class Component
	: public std::enable_shared_from_this<Component>
{
public:
	/// <summary>
	/// 所有者のアクター
	/// </summary>
	/// <param name="owner">アクター</param>
	Component(std::shared_ptr<Actor>owner);

	/// <summary>
	/// 所有者のActor取得
	/// </summary>
	std::weak_ptr<Actor> GetOwner();

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	virtual void Update() = 0;

private:
	std::weak_ptr<Actor> owner_;
};