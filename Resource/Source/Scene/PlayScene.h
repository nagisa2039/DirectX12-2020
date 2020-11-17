#pragma once
#include <memory>
#include "Scene.h"

class RendererManager;
class Player;
class Material;

class PlayScene :
	public Scene
{
public:
	PlayScene(SceneController& ctrl);
	~PlayScene();

	void Update()override;
	void Draw()override;

private:
	std::unique_ptr<RendererManager> rendererManager_;
	int tnktH_;
	int dmdnH_;
	int d3dH_;

	int BGMH_;
	int laserSEH_;
	int se1_;
	int se2_;

	std::unique_ptr<Player> player_;

	std::shared_ptr<Material> raymarchingMat_;
	std::shared_ptr<Material> mosaicMat_;
};