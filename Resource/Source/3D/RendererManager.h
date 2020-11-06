#pragma once
#include <memory>
#include <vector>

class Renderer;
class Dx12Wrapper;

class RendererManager
{
public:
	RendererManager(Dx12Wrapper& dx12);
	~RendererManager();

	void Update();
	void Draw();

private:
	Dx12Wrapper& dx12_;
	std::vector<std::shared_ptr<Renderer>> renderers_;

	int cameraScreenH_;
	int lightScreenH_;
};