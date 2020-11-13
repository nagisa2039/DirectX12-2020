#include "SceneController.h"
#include "PlayScene.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"
#include "3D/RendererManager.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"
#include "2D/SpriteDrawer.h"
#include "Utility/Constant.h"
#include "System/SoundManager.h"
#include "Utility/Input.h"
#include "Game/Player.h"

using namespace std;

PlayScene::PlayScene(SceneController & ctrl):Scene(ctrl)
{
	rendererManager_ = std::make_unique<RendererManager>(Application::Instance().GetDx12());
	auto& texLoader = Application::Instance().GetDx12().GetTexLoader();
	tnktH_ = texLoader.LoadGraph(L"Resource/Image/tnkt.png");
	dmdnH_ = texLoader.LoadGraph(L"Resource/Image/dmdn.jpg");

	auto wsize = Application::Instance().GetWindowSize();
	d3dH_ = texLoader.GetGraphHandle(D3D_CAMERA_VIEW_SCREEN);

	int tmpH = texLoader.MakeScreen(L"tmp", 1280, 720);

	auto& dx12 = Application::Instance().GetDx12();
	auto& soundManager = dx12.GetSoundManager();

	BGMH_ = soundManager.LoadWave(L"Resource/Sound/BGM/–ì—Ç”L‚Í‰F’ˆ‚ð–ÚŽw‚µ‚½.wav", true);
	player_ = make_unique<Player>();
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update()
{
	rendererManager_->Update();

	auto& dx12 = Application::Instance().GetDx12();
	auto& input = Application::Instance().GetInput();

	if (input.GetButtonDown(DIK_SPACE))
	{
		//auto& soundManager = dx12.GetSoundManager();
		//soundManager.PlayWave(BGMH_);	}
	}
	player_->Update();
}

void PlayScene::Draw()
{
	rendererManager_->Draw();

	auto& dx12 = Application::Instance().GetDx12();
	auto& texLoader = dx12.GetTexLoader();
	auto& spriteDrawer = dx12.GetSpriteDrawer();
	auto wsize = Application::Instance().GetWindowSize();

	int shadowH = texLoader.GetGraphHandle(D3D_LIGHT_VIEW_SCREEN);

	spriteDrawer.SetDrawScreen(dx12.GetBackScreenHandle());
	texLoader.ClsDrawScreen();
	spriteDrawer.SetDrawBright(255, 255, 255);

	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);

	spriteDrawer.SetPixelShader(L"Resource/Source/Shader/2D/PostEffect.hlsl");
	spriteDrawer.DrawGraph(0, 0, d3dH_);

	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);

	spriteDrawer.DrawRotaGraph(300, 300, 0.5f, 0.0f, tnktH_);
	spriteDrawer.DrawRotaGraph(300, 500, 0.5f, 0.0f, dmdnH_);
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	player_->Draw();

	spriteDrawer.End();

	dx12.ScreenFlip();
}