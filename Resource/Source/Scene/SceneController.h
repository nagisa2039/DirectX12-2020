#pragma once
#include <stack>
#include <memory>

class Scene;

/// <summary>
/// �V�[���Ǘ��N���X
/// </summary>
class SceneController
{
public:
	SceneController();
	~SceneController();

	/// <summary>
	/// �V�[���̍X�V
	/// </summary>
	void SceneUpdate();

	/// <summary>
	/// �V�[����ύX
	/// </summary>
	/// <param name="scene">�J�ڂ���V�[����unique_ptr</param>
	void ChangeScene(std::shared_ptr<Scene> scene);

	/// <summary>
	/// �V�[���̃X�^�b�N
	/// </summary>
	/// <param name="scene">�X�^�b�N����V�[����unique_ptr</param>
	void PushScene(std::shared_ptr<Scene> scene);

	/// <summary>
	/// �V�[���̃|�b�v
	/// ���ݍĐ����̃V�[�����폜��1�߂�
	/// </summary>
	void PopScene(void);

	/// <summary>
	/// ���݂̃V�[���Q�Ƃ̎擾
	/// </summary>
	Scene& GetCurrentScene();

private:
	std::deque<std::shared_ptr<Scene>> scene_;
};

