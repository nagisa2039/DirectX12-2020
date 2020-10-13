#pragma once
#include <memory>
#include "Scene.h"

class ModelRenderer;

class PlayScene :
	public Scene
{
public:
	PlayScene(SceneController & ctrl);
	~PlayScene();

	void Update()override;
	void Draw()override;

private:
	std::unique_ptr<ModelRenderer> modelRenderer_;
};