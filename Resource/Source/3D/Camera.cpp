#include "Camera.h"
#include "Utility/dx12Tool.h"
#include "System/Application.h"
#include "System/Command.h"
#include "Utility/Input.h"
#include "Utility/UtilityShaderStruct.h"

using namespace std;
using namespace DirectX;

namespace
{
	constexpr DirectX::XMFLOAT3 UP = { 0.0f, 1.0f, 0.0f };
}

Camera::Camera(Command& cmd, ID3D12Device& dev, std::weak_ptr<Actor>owner)
	:cmd_(cmd), dev_(dev), Component(owner)
{
	target_ = { 0, 10, 0 };
	fov_ = XMConvertToRadians(50.0f);
	mappedScene_ = nullptr;
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
	return target_;
}

void Camera::SetTargetPos(const DirectX::XMFLOAT3& tpos)
{
	target_ = tpos;
}

DirectX::XMFLOAT3 Camera::GetTargetVec() const
{
	auto pos = GetOwner().lock()->GetTransform().pos;
	return XMFLOAT3(target_.x - pos.x, target_.y - pos.y, target_.z - pos.z);
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	// カメラの更新
	auto eye = GetOwner().lock()->GetTransform().pos;
	XMVECTOR eyePos = XMLoadFloat3(&eye);
	XMVECTOR targetPos = XMLoadFloat3(&target_);
	XMVECTOR upVec = XMLoadFloat3(&UP);

	// カメラ用
	return XMMatrixLookAtLH(eyePos, targetPos, upVec);
}

DirectX::XMMATRIX Camera::GetProjMatrix() const
{
	auto wsize = Application::Instance().GetWindowSize();
	return XMMatrixPerspectiveFovLH(
		fov_, static_cast<float>(wsize.w) / static_cast<float>(wsize.h), 0.05f, 1000.0f);
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
	XMVECTOR targetPos = XMLoadFloat3(&target_);
	XMVECTOR upVec = XMLoadFloat3(&UP);
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
	trans.pos = { 0, 20, -30 };
	SetTransform(trans);
	camera_ = nullptr;
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
		camera_->SetTargetPos(XMFLOAT3(0, 10, 0));
	}

	auto& input = Application::Instance().GetInput();
	float moveSpeed = 1.0f;
	auto cameraMove = [&input = input](const char keycode, float& target, const float speed)
	{
		if (input.GetButton(keycode))
		{
			target += speed;
		}
	};

	auto transform = GetTransform();
	cameraMove(DIK_W, transform.pos.y, moveSpeed);
	cameraMove(DIK_S, transform.pos.y, -moveSpeed);
	cameraMove(DIK_D, transform.pos.x, moveSpeed);
	cameraMove(DIK_A, transform.pos.x, -moveSpeed);
	cameraMove(DIK_E, transform.pos.z, moveSpeed);
	cameraMove(DIK_Q, transform.pos.z, -moveSpeed);
	SetTransform(transform);

	/*auto targetPos = camera_->GetTargetPos();
	cameraMove(DIK_W, targetPos.y, moveSpeed);
	cameraMove(DIK_S, targetPos.y, -moveSpeed);
	cameraMove(DIK_D, targetPos.x, moveSpeed);
	cameraMove(DIK_A, targetPos.x, -moveSpeed);
	cameraMove(DIK_E, targetPos.z, moveSpeed);
	cameraMove(DIK_Q, targetPos.z, -moveSpeed);
	camera_->SetTargetPos(targetPos);*/

	Actor::Update();
}

std::shared_ptr<Camera>& CameraObject::GetCamera()
{
	return camera_;
}
