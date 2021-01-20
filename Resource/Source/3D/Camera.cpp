#include "Camera.h"
#include "System/Application.h"
#include "System/Command.h"
#include "Utility/Input.h"
#include "Utility/UtilityShaderStruct.h"
#include "Utility/ImGuiTool.h"
#include "Utility/Cast.h"
#include "Utility/dx12Tool.h"

using namespace std;
using namespace DirectX;

Camera::Camera(Command& cmd, ID3D12Device& dev, std::weak_ptr<Actor>owner)
	:cmd_(cmd), dev_(dev), Component(owner)
{
	fov_ = 60.0f;
	mappedScene_ = nullptr;

	projectionMode_ = ProjectionMode::Perspective;

	projectionFuncTable_[Uint64(ProjectionMode::Perspective)]
		= [](float fov, float w, float h)
	{
		return XMMatrixPerspectiveFovLH(
			XMConvertToRadians(fov), w / h, 0.05f, 1000.0f);
	};
	projectionFuncTable_[Uint64(ProjectionMode::Orthographic)]
		= [](float fov, float w, float h)
	{
		float view = 80;
		return XMMatrixOrthographicLH(view * w / h, view, 0.05f, 1000.0f);
	};
	CreateCameraConstantBufferAndView();
	UpdateCamera();
}

Camera::~Camera()
{
}

void Camera::Init()
{
}

void Camera::Update()
{
	UpdateCamera();
}

void Camera::SetCameraDescriptorHeap(const UINT rootParamIdx)
{
	// カメラ用デスクリプタヒープの設定
	auto& commandList = cmd_.CommandList();
	commandList.SetDescriptorHeaps(1, cameraHeap_.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(rootParamIdx,
		cameraHeap_->GetGPUDescriptorHandleForHeapStart());
}

DirectX::XMFLOAT3 Camera::GetTargetPos() const
{
	auto& trans = GetOwner().lock()->GetTransform();
	DirectX::XMFLOAT3 targetVecF3;
	XMStoreFloat3(&targetVecF3, trans.GetForwerd());
	return DirectX::XMFLOAT3{	trans.pos.x + targetVecF3.x, 
								trans.pos.y + targetVecF3.y, 
								trans.pos.z + targetVecF3.z };
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	// カメラの更新
	auto eye = GetOwner().lock()->GetTransform().pos;
	XMVECTOR eyePos = XMLoadFloat3(&eye);
	auto targetPosF3 = GetTargetPos();
	XMVECTOR targetPos = XMLoadFloat3(&targetPosF3);

	// カメラ用
	return XMMatrixLookAtLH(eyePos, targetPos, GetOwner().lock()->GetTransform().GetUp());
}

DirectX::XMMATRIX Camera::GetProjMatrix() const
{
	auto wsize = Application::Instance().GetWindowSize();
	return projectionFuncTable_[Uint64(projectionMode_)](fov_, Float(wsize.w), Float(wsize.h));
}

void Camera::DrawImGui()
{
	ImGui::DragFloat("FOV", &fov_, 0.1f, 1.0f, 180.0f);
	fov_ = std::clamp(fov_, 1.0f, 180.0f);
}

bool Camera::CreateCameraConstantBufferAndView()
{
	CreateBuffer(&dev_, cameraCB_, D3D12_HEAP_TYPE_UPLOAD, sizeof(*mappedScene_));
	cameraCB_->Map(0, nullptr, (void**)&mappedScene_);
	mappedScene_->lightVec = XMFLOAT3(1.0f, -1.0f, 1.0f);

	CreateDescriptorHeap(&dev_, cameraHeap_);

	// 定数バッファビューの作成
	CreateConstantBufferView(&dev_, cameraCB_, cameraHeap_->GetCPUDescriptorHandleForHeapStart());

	return true;
}

void Camera::UpdateCamera()
{
	auto eye = GetOwner().lock()->GetTransform().pos;
	XMVECTOR eyePos = XMLoadFloat3(&eye);
	auto targetPosF3 = GetTargetPos();
	XMVECTOR targetPos = XMLoadFloat3(&targetPosF3);
	XMVECTOR upVec = GetOwner().lock()->GetTransform().GetUp();
	XMVECTOR lightVec = XMLoadFloat3(&mappedScene_->lightVec);

	auto cameraArmLength = XMVector3Length(XMVectorSubtract(targetPos, eyePos)).m128_f32[0];
	XMVECTOR lightCamPos = targetPos - lightVec * cameraArmLength;

	// ライト用
	auto lightView = XMMatrixLookAtLH(lightCamPos, targetPos, upVec);
	auto lightProj = XMMatrixOrthographicLH(80, 80, 0.05f, 1000.0f);

	mappedScene_->view = GetViewMatrix();
	mappedScene_->proj = GetProjMatrix();
	mappedScene_->eye = GetOwner().lock()->GetTransform().pos;
	mappedScene_->invProj = XMMatrixInverse(nullptr, mappedScene_->proj);
	mappedScene_->lightCamera = lightView * lightProj;
}

CameraObject::CameraObject(Command& cmd, ID3D12Device& dev)
	:cmd_(cmd), dev_(dev)
{
	auto trans = GetTransform();
	trans.pos = { 0, 15, -30 };
	SetTransform(trans);
	camera_ = nullptr;
	SetName("Camera");
}

CameraObject::~CameraObject()
{
}

void CameraObject::Update()
{
	if (!camera_)
	{
		camera_ = make_shared<Camera>(cmd_, dev_, shared_from_this());
		AddComponent(camera_);
	}

	auto& input = Application::Instance().GetInput();
	auto transform = GetTransform();
	float moveSpeed = 1.0f;
	XMVECTOR pos = XMLoadFloat3(&transform.pos);
	auto cameraMove = [&input = input, &pos = pos, moveSpeed]
		(const char keycode, const XMVECTOR& dir)
	{
		if (input.GetButton(keycode))
		{
			pos += dir * moveSpeed;
		}
	};

	auto forwerd	= transform.GetForwerd();
	auto up			= transform.GetUp();
	auto right		= transform.GetRight();

	cameraMove(DIK_W,  up);
	cameraMove(DIK_S, -up);
	cameraMove(DIK_D,  right);
	cameraMove(DIK_A, -right);
	cameraMove(DIK_E,  forwerd);
	cameraMove(DIK_Q, -forwerd);

	XMStoreFloat3(&transform.pos, pos);
	SetTransform(transform);

	Actor::Update();
}

std::shared_ptr<Camera>& CameraObject::GetCamera()
{
	return camera_;
}
