#pragma once
#include <memory>
#include <vector>
#include "Scene.h"

class RendererManager;
class Material;
class Actor;

/// <summary>
/// �Đ��V�[��
/// </summary>
class PlayScene :
	public Scene
{
public:
	/// <param name="ctrl">�V�[���Ǘ��N���X</param>
	PlayScene(SceneController& ctrl);
	~PlayScene();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update()override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw()override;

private:
	int cameraH_;
	int hitoshashiH_;
	int d3dH_;

	int BGMH_;
	int laserSEH_;
	int se1_;
	int se2_;

	int skyH_;

	std::vector<std::shared_ptr<Actor>> actors_;

	std::shared_ptr<Material> raymarchingMat_;
	std::shared_ptr<Material> skySphereMat_;
	std::shared_ptr<Material> mosaicMat_;
};