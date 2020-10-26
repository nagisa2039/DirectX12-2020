#include "SceneController.h"
#include "PlayScene.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"
#include "Model/ModelRenderer.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "System/SpriteDrawer.h"
#include "Utility/Constant.h"
#include "System/SoundManager.h"

using namespace std;

PlayScene::PlayScene(SceneController & ctrl):Scene(ctrl)
{
	modelRenderer_ = std::make_unique<ModelRenderer>(Application::Instance().GetDx12());
	auto& texLoader = Application::Instance().GetDx12().GetTexLoader();
	tnktH_ = texLoader.LoadGraph(L"Resource/Image/tnkt.png");
	dmdnH_ = texLoader.LoadGraph(L"Resource/Image/dmdn.jpg");

	auto wsize = Application::Instance().GetWindowSize();
	d3dH_ = texLoader.GetGraphHandle(D3D_SPACE_SCREEN);

	int tmpH = texLoader.MakeScreen(L"tmp", 1280, 720);

	auto& dx12 = Application::Instance().GetDx12();
	auto& soundManager = dx12.GetSoundManager();
	//int waveH = soundManager.LoadWave(L"Resource/Sound/SE/laser1.wav");
	//soundManager.PlayWave(waveH);
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
	modelRenderer_->DrawTo3DSpace();

	auto& dx12 = Application::Instance().GetDx12();
	auto& texLoader = dx12.GetTexLoader();
	auto& spriteDrawer = dx12.GetSpriteDrawer();
	auto wsize = Application::Instance().GetWindowSize();

	int tmpH = texLoader.LoadGraph(L"tmp");
	spriteDrawer.SetDrawScreen(tmpH);
	texLoader.ClsDrawScreen();

	spriteDrawer.SetDrawBright(255, 255, 0);
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	spriteDrawer.DrawGraph(0, 0, d3dH_);

	spriteDrawer.SetDrawBright(255, 255, 255);
	spriteDrawer.SetDrawScreen(dx12.GetBackScreenHandle());
	texLoader.ClsDrawScreen();

	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	spriteDrawer.DrawExtendGraph(0,0,wsize.w, wsize.h, dmdnH_);
	spriteDrawer.DrawGraph(0, 0, tmpH);
	spriteDrawer.SetDrawBlendMode(BlendMode::add, 255);
	spriteDrawer.DrawGraph(0, 0, tmpH);
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	spriteDrawer.SetDrawBright(255, 255, 255);
	spriteDrawer.DrawGraph(0, 0, tnktH_);
	spriteDrawer.SetDrawBlendMode(BlendMode::add, 255);
	spriteDrawer.DrawGraph(0, 0, tnktH_);
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);

	spriteDrawer.End();

	dx12.ScreenFlip();
}