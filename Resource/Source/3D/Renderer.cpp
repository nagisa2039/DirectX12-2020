#include "Renderer.h"
#include "Camera.h"

Renderer::Renderer(std::shared_ptr<SceneInf>& sceneInf)
	:sceneInf_(sceneInf)
{
}

void Renderer::ComputeUpdate(std::vector<Mesh*>& meshs)
{
}