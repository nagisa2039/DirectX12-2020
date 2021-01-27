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
#include "Material/StanderedMaterial.h"
#include "3D/Actor.h"
#include "3D/Skeletal/SkeletalMesh.h"
#include "3D/Static/PlaneMesh.h"
#include "Utility/Cast.h"
#include "Imgui/imgui.h"
#include <sstream>

using namespace std;
using namespace DirectX;

namespace
{
	bool rayMarching;
	bool debugDraw;
}

PlayScene::PlayScene(SceneController & ctrl):Scene(ctrl)
{
	auto& texLoader = Application::Instance().GetDx12().GetTexLoader();

	auto wsize = Application::Instance().GetWindowSize();
	d3dH_ = texLoader.GetGraphHandle(SCR_CAMERA_VIEW_SCREEN);

	mosaicH_ = texLoader.MakeScreen(L"mosaic", 1280, 720);

	rayMarching = true;
	debugDraw = true;

	auto& dx12 = Application::Instance().GetDx12();
	auto& soundManager = dx12.GetSoundManager();

	BGMH_ = soundManager.LoadWave(L"Resource/Sound/BGM/BGM1.wav", true);

	raymarchingMat_ = make_shared<StanderedMaterial>(L"Resource/Source/Shader/2D/Raymarching.hlsl");
	skySphereMat_ = make_shared<StanderedMaterial>(L"Resource/Source/Shader/2D/SkySphere.hlsl");
	mosaicMat_ = make_shared<StanderedMaterial>(L"Resource/Source/Shader/2D/Mosaic.hlsl");

	actors_.reserve(10);
	auto AddPlaneActor = [&]()
	{
		auto actor = make_shared<Actor>();
		auto mesh = make_shared<PlaneMesh>(actor, dx12, XMFLOAT3(0.0f, 0.0f, 0.0f), 150.0f, 80.0f, L"Resource/Image/floor.jpg");
		actor->AddComponent(mesh);
		actor->SetName("Plane");

		actors_.emplace_back(actor);
	};

	auto AddMMDActor = [&](const std::wstring& modelFilePath, const std::wstring& motionFilePath)
	{
		auto actor = make_shared<Actor>();
		auto mesh = make_shared<SkeletalMesh>(actor, dx12, modelFilePath, motionFilePath);
		actor->AddComponent(mesh);
		actor->SetName("MMDModel");
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
	if (rayMarching)
	{
		spriteDrawer.SetMaterial(raymarchingMat_);
		spriteDrawer.DrawGraph(0, 0, d3dH_);
	}
	else
	{
		spriteDrawer.SetMaterial(skySphereMat_);
		spriteDrawer.DrawGraph(0, 0, texLoader.LoadGraph(L"Resource/Image/sky.png"));
	}

	// 3D描画
	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);
	spriteDrawer.DrawGraph(0, 0, d3dH_);

	if (debugDraw)
	{
		std::list<int> debugDrawHandles;
		debugDrawHandles.emplace_back(texLoader.GetGraphHandle(SCR_COLOR_SHRINK));
		debugDrawHandles.emplace_back(texLoader.GetGraphHandle(SCR_EMMISION_SHRINK));
		debugDrawHandles.emplace_back(texLoader.GetGraphHandle(SCR_CAMERA_MR_COLOR));
		debugDrawHandles.emplace_back(texLoader.GetGraphHandle(SCR_CAMERA_MR_NORMAL));
		debugDrawHandles.emplace_back(texLoader.GetGraphHandle(SCR_CAMERA_MR_BRIGHT));
		float aspect = wsize.w / static_cast<float>(wsize.h);
		int hight = 100;
		XMINT2 size = XMINT2(Int32(hight * aspect), hight);
		for (int i = 0; const auto& handle : debugDrawHandles)
		{
			spriteDrawer.DrawExtendGraph(0, hight * i, size.x, hight * i + size.y, handle);
			i++;
		}
	}

	spriteDrawer.SetDrawBlendMode(BlendMode::noblend, 255);

	spriteDrawer.End();

	dx12.BeginDrawImGui();

	auto& soundManager = dx12.GetSoundManager();
	if (soundManager.CheckPlaySound(BGMH_))
	{
		if (ImGui::Button("BGM : Stop"))
		{
			soundManager.StopSound(BGMH_);
		}
	}
	else
	{
		if (ImGui::Button("BGM : Play"))
		{
			soundManager.PlayWave(BGMH_);
		}
	}
	ImGui::Checkbox("RayMarching", &rayMarching);
	ImGui::Checkbox("DebugDraw", &debugDraw);

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Models"))
	{
		for (int num = 0; auto & actor : actors_)
		{
			actor->DrawImGui(num);
			num++;
		}
		ImGui::TreePop();
	}

	dx12.EndDrawImGui();

	dx12.ScreenFlip();
}