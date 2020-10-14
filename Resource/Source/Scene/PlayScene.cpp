#include "SceneController.h"
#include "PlayScene.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"
#include "Model/ModelRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "System/SpriteDrawer.h"

using namespace std;

PlayScene::PlayScene(SceneController & ctrl):Scene(ctrl)
{
	modelRenderer_ = std::make_unique<ModelRenderer>(Application::Instance().GetDx12());
	auto& texLoader = Application::Instance().GetDx12().GetTexLoader();
	_tnktH = texLoader.LoadGraph("Resource/Image/tnkt.png");
	_dmdnH = texLoader.LoadGraph("Resource/Image/dmdn.jpg");
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
	modelRenderer_->DrawToMyScreen();

	auto& dx12 = Application::Instance().GetDx12();
	auto& texLoader = dx12.GetTexLoader();
	auto& spriteDrawer = dx12.GetSpriteDrawer();

	texLoader.SetDrawScreen(dx12.GetBackScreenHandle());
	texLoader.ClsDrawScreen();
	
	static float angle = 0.0f;

	angle += 1.0f;

	modelRenderer_->Draw();
	spriteDrawer.DrawGraph(0,0, _tnktH);
	spriteDrawer.DrawRotaGraph(1280/2, 720/2, 1.5f, angle * 3.14f / 180.0f, _dmdnH);
	spriteDrawer.DrawRectGraph(1280-500, 720-500, 250, 250, 100, 100, _tnktH);
	spriteDrawer.End();

	dx12.ScreenFlip();
}