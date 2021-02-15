#include "SceneInf.h"
#include "System/Command.h"
#include "Utility/dx12Tool.h"
#include "Shader/Struct/UtilityShaderStruct.h"
#include "Camera.h"

using namespace std;
using namespace DirectX;

SceneInf::SceneInf(Command& cmd, ID3D12Device& dev):cmd_(cmd), dev_(dev)
{
	mappedScene_ = nullptr;
	CreateCameraConstantBufferAndView();


}

SceneInf::~SceneInf()
{
}

void SceneInf::Init()
{
	cameraObj_ = make_shared<CameraObject>(true);
	cameraObj_->Init(Camera::ProjectionMode::Perspective);
	cameraObj_->GetTransform().pos = { 0, 15, -30 };
	cameraObj_->SetName("Camera");

	lightObj_ = make_shared<CameraObject>(false);
	lightObj_->Init(Camera::ProjectionMode::Orthographic);
	auto& trans = lightObj_->GetTransform();
	trans.pos = {-30, 30, -30};
	trans.rotate = { 45, 45, 0 };
	lightObj_->SetName("Light");
}

void SceneInf::Update()
{
	cameraObj_->Update();
	lightObj_->Update();

	auto camera = cameraObj_->GetCamera();
	auto light = lightObj_->GetCamera();

	mappedScene_->view = camera->GetViewMatrix();
	mappedScene_->proj = camera->GetProjMatrix();
	mappedScene_->eye = camera->GetOwner().lock()->GetTransform().pos;
	mappedScene_->invProj = XMMatrixInverse(nullptr, mappedScene_->proj);
	mappedScene_->lightCamera = light->GetViewMatrix() * light->GetProjMatrix();
	mappedScene_->cameraRotate = cameraObj_->GetTransform().GetRotateMatrix();
	XMStoreFloat3(&mappedScene_->lightVec, lightObj_->GetTransform().GetForwerd());
}

void SceneInf::SetCameraDescriptorHeap(const unsigned int rootParamIdx)
{
	// カメラ用デスクリプタヒープの設定
	auto& commandList = cmd_.CommandList();
	commandList.SetDescriptorHeaps(1, cameraHeap_.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(rootParamIdx,
		cameraHeap_->GetGPUDescriptorHandleForHeapStart());
}

void SceneInf::DrawImGui()
{
	cameraObj_->DrawImGui(0);
	mappedScene_->fov = cameraObj_->GetCamera()->GetFOV();
	lightObj_->DrawImGui(1);
}

std::shared_ptr<CameraObject>& SceneInf::GetCameraObject()
{
	return cameraObj_;
}

bool SceneInf::CreateCameraConstantBufferAndView()
{
	CreateBuffer(&dev_, cameraCB_, D3D12_HEAP_TYPE_UPLOAD, sizeof(*mappedScene_));
	cameraCB_->Map(0, nullptr, (void**)&mappedScene_);
	mappedScene_->lightVec = { 1.0f, -1.0f, 1.0f };

	CreateDescriptorHeap(&dev_, cameraHeap_);

	// 定数バッファビューの作成
	CreateConstantBufferView(&dev_, cameraCB_, cameraHeap_->GetCPUDescriptorHandleForHeapStart());

	return true;
}