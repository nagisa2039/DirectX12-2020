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
#include "Utility/Input.h"
#include "Game/Player.h"

using namespace std;

PlayScene::PlayScene(SceneController & ctrl):Scene(ctrl)
{
	modelRenderer_ = std::make_unique<ModelRenderer>(Application::Instance().GetDx12());
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
	modelRenderer_->Update();

	auto& dx12 = Application::Instance().GetDx12();
	auto& input = Application::Instance().GetInput();

	auto cameraPos = dx12.GetCameraPosition();
	float moveSpeed = 1.0f;
	auto cameraMove = [&input = input](const char keycode, float& target, const float speed)
	{
		if (input.GetButton(keycode))
		{
			target += speed;
		}
	};

	cameraMove(DIK_W, cameraPos.y, moveSpeed);
	cameraMove(DIK_S, cameraPos.y, -moveSpeed);
	cameraMove(DIK_D, cameraPos.x, moveSpeed);
	cameraMove(DIK_A, cameraPos.x, -moveSpeed);
	cameraMove(DIK_E, cameraPos.z, moveSpeed);
	cameraMove(DIK_Q, cameraPos.z, -moveSpeed);
	dx12.SetCameraPosision(cameraPos);
	dx12.UpdateCamera();

	if (input.GetButtonDown(DIK_SPACE))
	{
		//auto& soundManager = dx12.GetSoundManager();
		//soundManager.PlayWave(BGMH_);	}
	}
	player_->Update();
}

void PlayScene::Draw()
{
	modelRenderer_->DrawTo3DSpace();

	auto& dx12 = Application::Instance().GetDx12();
	auto& texLoader = dx12.GetTexLoader();
	auto& spriteDrawer = dx12.GetSpriteDrawer();
	auto wsize = Application::Instance().GetWindowSize();

	int shadowH = texLoader.LoadGraph(D3D_LIGHT_VIEW_SCREEN);

	spriteDrawer.SetDrawScreen(dx12.GetBackScreenHandle());
	texLoader.ClsDrawScreen();
	spriteDrawer.SetDrawBright(255, 255, 255);
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	spriteDrawer.DrawGraph(0, 0, d3dH_);
	spriteDrawer.DrawRotaGraph(300, 300, 0.5f, 0.0f, tnktH_);
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	spriteDrawer.DrawExtendGraph(0, 0, 100, 1280 / 720.0f * 100.0f, shadowH);

	player_->Draw();

	spriteDrawer.End();

	dx12.ScreenFlip();
}