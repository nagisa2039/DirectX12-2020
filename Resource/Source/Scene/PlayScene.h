#pragma once
#include <memory>
#include <vector>
#include "Scene.h"

class RendererManager;
class Material;
class Actor;

class PlayScene :
	public Scene
{
public:
	PlayScene(SceneController& ctrl);
	~PlayScene();

	void Update()override;
	void Draw()override;

private:
	int cameraH_;
	int hitoshashiH_;
	int d3dH_;

	int BGMH_;
	int laserSEH_;
	int se1_;
	int se2_;

	int mosaicH_;

	int skyH_;

	std::vector<std::shared_ptr<Actor>> actors_;

	std::shared_ptr<Material> raymarchingMat_;
	std::shared_ptr<Material> skySphereMat_;
	std::shared_ptr<Material> mosaicMat_;
};