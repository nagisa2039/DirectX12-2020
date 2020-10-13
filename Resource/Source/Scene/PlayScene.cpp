#include "SceneController.h"
#include "PlayScene.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"
#include "Model/ModelRenderer.h"
#include "System/Dx12Wrapper.h"

using namespace std;

PlayScene::PlayScene(SceneController & ctrl):Scene(ctrl)
{
	modelRenderer_ = std::make_unique<ModelRenderer>(Application::Instance().GetDx12());
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update()
{
	modelRenderer_->Update();
}

void PlayScene::Draw()
{
	auto& dx12 = Application::Instance().GetDx12();
	
	dx12.SetDrawScreen(dx12.GetBackScreenHandle());
	dx12.ClsDrawScreen();

	int handle = dx12.LoagGraph("Resource/Image/tnkt.png");
	dx12.DrawGraph(0, 0, handle);
	dx12.DrawEnd();

	modelRenderer_->Draw();

	dx12.ScreenFlip();
}