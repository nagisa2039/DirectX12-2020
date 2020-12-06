#include "Renderer.h"
#include "Camera.h"

Renderer::Renderer(std::shared_ptr<Camera>& camera)
	:camera_(camera)
{
}

void Renderer::ComputeUpdate(std::vector<Mesh*>& meshs)
{
}