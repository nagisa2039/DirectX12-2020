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
#include "Material/StanderedMaterial.h"
#include "3D/Actor.h"
#include "3D/Skeletal/SkeletalMesh.h"
#include "3D/Static/PlaneMesh.h"
#include "Utility/Cast.h"

using namespace std;
using namespace DirectX;

PlayScene::PlayScene(SceneController & ctrl):Scene(ctrl)
{
	auto& texLoader = Application::Instance().GetDx12().GetTexLoader();

	auto wsize = Application::Instance().GetWindowSize();
	d3dH_ = texLoader.GetGraphHandle(D3D_CAMERA_VIEW_SCREEN);

	mosaicH_ = texLoader.MakeScreen(L"mosaic", 1280, 720);

	auto& dx12 = Application::Instance().GetDx12();
	auto& soundManager = dx12.GetSoundManager();

	//BGMH_ = soundManager.LoadWave(L"Resource/Sound/BGM/野良猫は宇宙を目指した.wav", true);
	player_ = make_unique<Player>();

	raymarchingMat_ = make_shared<StanderedMaterial>(L"Resource/Source/Shader/2D/Raymarching.hlsl");
	mosaicMat_ = make_shared<StanderedMaterial>(L"Resource/Source/Shader/2D/Mosaic.hlsl");

	actors_.reserve(10);
	auto AddPlaneActor = [&]()
	{
		auto actor = make_shared<Actor>();
		auto mesh = make_shared<PlaneMesh>(actor, dx12, XMFLOAT3(0.0f, 0.0f, 0.0f), (1000.0f / 536.0f) * 80.0f, 80.0f, L"image/fiona.png");
		actor->AddComponent(mesh);
		actors_.emplace_back(actor);
	};

	auto AddMMDActor = [&](const std::wstring& modelFilePath, const std::wstring& motionFilePath)
	{
		auto actor = make_shared<Actor>();
		auto mesh = make_shared<SkeletalMesh>(actor, dx12, modelFilePath, motionFilePath);
		actor->AddComponent(mesh);
		actors_.emplace_back(actor);
	};

	AddMMDActor(L"Resource/Model/ぽんぷ長式神風/ぽんぷ長式神風.pmx", L"Resource/VMD/swing2.vmd");
	AddMMDActor(L"Resource/Model/ぽんぷ長式村雨/ぽんぷ長式村雨.pmx", L"Resource/VMD/ヤゴコロダンス.vmd");
	AddMMDActor(L"Resource/Model/葛城/葛城.pmd", L"Resource/VMD/ヤゴコロダンス.vmd");
	AddMMDActor(L"Resource/Model/桜ミク/雪ミク.pmd", L"Resource/VMD/swing2.vmd");
	AddMMDActor(L"Resource/Model/桜ミク/mikuXS桜ミク.pmd", L"Resource/VMD/swing2.vmd");
	for (int i = 0; auto & actor : actors_)
	{
		int moveZ = (i + 1) / 2;
		int moveX = moveZ * ((i % 2) * 2 - 1);

		auto trans = actor->GetTransform();
		trans.pos = XMFLOAT3(8.0f * moveX, 0.0f, 5.0f * moveZ);
		actor->SetTransform(trans);
		i++;
	}

	AddPlaneActor();
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update()
{
	auto& dx12 = Application::Instance().GetDx12();
	auto& input = Application::Instance().GetInput();


	if (input.GetButtonDown(DIK_SPACE))
	{
		//auto& soundManager = dx12.GetSoundManager();
		//soundManager.PlayWave(BGMH_);	}
	}
	player_->Update();

	for (auto& actor : actors_)
	{
		actor->Update();
	}

	dx12.GetRendererManager().Update();
}

void PlayScene::Draw()
{
	auto& dx12 = Application::Instance().GetDx12();
	auto& texLoader = dx12.GetTexLoader();
	auto& spriteDrawer = dx12.GetSpriteDrawer();
	auto wsize = Application::Instance().GetWindowSize();

	dx12.GetRendererManager().Draw();

	// mosaic画像生成
	spriteDrawer.SetDrawScreen(mosaicH_);
	texLoader.ClsDrawScreen();
	spriteDrawer.SetMaterial(mosaicMat_);
	spriteDrawer.DrawGraph(0, 0, d3dH_);

	spriteDrawer.SetDrawScreen(dx12.GetBackScreenHandle());
	texLoader.ClsDrawScreen();
	spriteDrawer.SetDrawBright(255, 255, 255);

	// レイマーチング
	spriteDrawer.SetMaterial(raymarchingMat_);
	spriteDrawer.DrawGraph(0, 0, d3dH_);

	// 3D描画
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	spriteDrawer.DrawGraph(0, 0, d3dH_);

	float aspect = wsize.w / static_cast<float>(wsize.h);
	XMINT2 size = XMINT2(Int32(100 * aspect), 100);
	spriteDrawer.DrawExtendGraph(0, 200, size.x, 200 + size.y, texLoader.GetGraphHandle(D3D_CAMERA_SHRINK_SCREEN));
	spriteDrawer.DrawExtendGraph(0, 300, size.x, 300 + size.y, texLoader.GetGraphHandle(D3D_CAMERA_MR_COLOR));
	spriteDrawer.DrawExtendGraph(0, 400, size.x, 400 + size.y, texLoader.GetGraphHandle(D3D_CAMERA_MR_NORMAL));
	spriteDrawer.DrawExtendGraph(0, 500, size.x, 500 + size.y, texLoader.GetGraphHandle(D3D_CAMERA_MR_BRIGHT));
	
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);

	spriteDrawer.End();
	//dx12.DrawEfk();

	dx12.ScreenFlip();
}