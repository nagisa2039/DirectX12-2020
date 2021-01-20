#pragma once
#include <vector>
#include <memory>

class Mesh;
class SceneInf;

/// <summary>
/// 3D�`��N���X�̃C���^�[�t�F�[�X
/// </summary>
class Renderer
{
public:
	/// <param name="sceneInf">�V�[�����Ǘ��N���X</param>
	Renderer(std::shared_ptr<SceneInf>& sceneInf);

	/// <summary>
	/// Compute�ɂ��X�V
	/// Renderer����ĂԂ��߂�Update���番��
	/// </summary>
	virtual void ComputeUpdate(std::vector<Mesh*>& meshs);

	/// <summary>
	/// �ʏ�`��
	/// </summary>
	/// <param name="meshs">�`�悷�郁�b�V��</param>
	virtual void Draw(std::vector<Mesh*>& meshs) = 0;

	/// <summary>
	/// �e�`��
	/// </summary>
	/// <param name="meshs">�`�悷�郁�b�V��</param>
	virtual void DrawShadow(std::vector<Mesh*>& meshs)=0;

protected:
	std::shared_ptr<SceneInf>& sceneInf_;
};

