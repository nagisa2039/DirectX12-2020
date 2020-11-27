#pragma once
#include "3D/Component.h"

class Actor;
class Dx12Wrapper;

/// <summary>
/// ���b�V�����N���X
/// </summary>
class Mesh
	:public Component
{
public:
	// ���b�V���̎��
	enum class Type
	{
		// �{�[���������b�V��
		static_mesh,
		// �{�[���L���b�V��
		skeletal_mesh,

		max
	};

	/// <param name="dx12">DirectX12�Ǘ��N���X</param>
	/// <param name="owner">���L��</param>
	/// <param name="type">���b�V���^�C�v</param>
	/// <param name=""></param>
	Mesh(Dx12Wrapper& dx12, std::weak_ptr<Actor>owner, const Mesh::Type type);
	virtual ~Mesh();

	/// <summary>
	/// ������
	/// �R���X�g���N�^�łł��Ȃ�(���g������Ȃ��Əo���Ȃ�)
	/// �������s��
	/// </summary>
	virtual void Init()override;

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

protected:
	// DirectX12�Ǘ��N���X
	Dx12Wrapper& dx12_;

private:
	const Mesh::Type type_;
};

