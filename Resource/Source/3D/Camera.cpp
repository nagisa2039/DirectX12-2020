#include "Camera.h"
#include "System/Application.h"
#include "System/Command.h"
#include <DirectXMath.h>
#include "Utility/Input.h"
#include "Shader/Struct/UtilityShaderStruct.h"
#include "Utility/ImGuiTool.h"
#include "Utility/Cast.h"
#include <algorithm>

using namespace std;
using namespace DirectX;

Camera::Camera(std::weak_ptr<Actor>owner)
	:Component(owner)
{
	fov_ = 60.0f;

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
}

Camera::~Camera()
{
}

void Camera::Init()
{
}

void Camera::Update()
{
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

float Camera::GetFOV() const
{
	return fov_;
}

void Camera::DrawImGui()
{
	ImGui::DragFloat("FOV", &fov_, 0.1f, 1.0f, 180.0f);
	fov_ = std::clamp(fov_, 1.0f, 180.0f);
}

void Camera::SetProjectionMode(const ProjectionMode pm)
{
	projectionMode_ = pm;
}

CameraObject::CameraObject(const bool keybordMove):keybordMove_(keybordMove)
{
	camera_ = nullptr;
}

CameraObject::~CameraObject()
{
}

void CameraObject::Init(const Camera::ProjectionMode pm)
{
	camera_ = make_shared<Camera>(shared_from_this());
	camera_->SetProjectionMode(pm);
	AddComponent(camera_);
}

void CameraObject::Update()
{
	if (!keybordMove_)return;

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
