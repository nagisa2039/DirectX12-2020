#pragma once
#include <stack>
#include <memory>

class Scene;

class SceneController
{
public:
	SceneController();
	~SceneController();

	// �V�[���̍X�V
	void SceneUpdate();

	// �V�[����ύX
	// @param scene	�J�ڂ���V�[����unique_ptr
	void ChangeScene(std::shared_ptr<Scene> scene);

	// �V�[���̃X�^�b�N
	// @param scene	�X�^�b�N����V�[����unique_ptr
	void PushScene(std::shared_ptr<Scene> scene);

	// �V�[���̃|�b�v
	void PopScene(void);

	/// <summary>
	/// ���݂̃V�[���Q�Ƃ̎擾
	/// </summary>
	Scene& GetCurrentScene();

private:
	std::deque<std::shared_ptr<Scene>> scene_;
};

