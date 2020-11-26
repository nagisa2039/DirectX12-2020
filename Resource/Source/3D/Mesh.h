#pragma once
#include "3D/Component.h"
class Actor;

/// <summary>
/// ���b�V�����N���X
/// </summary>
class Mesh
	:public Component
{
public:
	enum class Type
	{
		// �{�[���������b�V��
		static_mesh,
		// �{�[���L���b�V��
		skeletal_mesh,

		max
	};

	/// <param name="type">���b�V���^�C�v</param>
	/// <param name="owner">���L��</param>
	Mesh(const Mesh::Type type, std::shared_ptr<Actor>owner);
	~Mesh()=default;

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update()=0;

	/// <summary>
	/// �ʏ�`��
	/// </summary>
	virtual void Draw()=0;

	/// <summary>
	/// ���b�V���^�C�v�̎擾
	/// </summary>
	Mesh::Type GetMeshType()const;

private:
	const Mesh::Type type_;
};

