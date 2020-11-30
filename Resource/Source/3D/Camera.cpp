#include "Camera.h"
#include "Utility/dx12Tool.h"
#include "System/Application.h"
#include "System/Command.h"
#include "Utility/Input.h"
#include "Utility/UtilityShaderStruct.h"

using namespace std;
using namespace DirectX;

Camera::Camera(Command& cmd, ID3D12Device& dev):cmd_(cmd), dev_(dev)
{
	eye_ = { 0, 20, -30 };
	target_ = { 0, 10, 0 };
	up_ = { 0, 1, 0 };
	fov_ = XMConvertToRadians(50.0f);
	mappedScene_ = nullptr;
	CreateCameraConstantBufferAndView();
	UpdateCamera();
}

Camera::~Camera()
{
}

void Camera::Update()
{
	auto& input = Application::Instance().GetInput();
	float moveSpeed = 1.0f;
	auto cameraMove = [&input = input](const char keycode, float& target, const float speed)
	{
		if (input.GetButton(keycode))
		{
			target += speed;
		}
	};

	cameraMove(DIK_W, eye_.y, moveSpeed);
	cameraMove(DIK_S, eye_.y, -moveSpeed);
	cameraMove(DIK_D, eye_.x, moveSpeed);
	cameraMove(DIK_A, eye_.x, -moveSpeed);
	cameraMove(DIK_E, eye_.z, moveSpeed);
	cameraMove(DIK_Q, eye_.z, -moveSpeed);

	cameraMove(DIK_W, target_.y, moveSpeed);
	cameraMove(DIK_S, target_.y, -moveSpeed);
	cameraMove(DIK_D, target_.x, moveSpeed);
	cameraMove(DIK_A, target_.x, -moveSpeed);
	cameraMove(DIK_E, target_.z, moveSpeed);
	cameraMove(DIK_Q, target_.z, -moveSpeed);

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

DirectX::XMFLOAT3 Camera::GetCameraPosition() const
{
	return DirectX::XMFLOAT3(eye_.x, eye_.y, eye_.z);
}

DirectX::XMFLOAT3 Camera::GetCameraTarget() const
{
	return DirectX::XMFLOAT3(target_.x, target_.y, target_.z);
}

DirectX::XMFLOAT3 Camera::GetLightVec() const
{
	return mappedScene_->lightVec;
}

void Camera::SetCameraPosision(const DirectX::XMFLOAT3& pos)
{
	eye_ = pos;
}

void Camera::SetCameraTarget(const DirectX::XMFLOAT3& target)
{
	target_ = target;
}

bool Camera::CreateCameraConstantBufferAndView()
{
	CreateUploadBuffer(&dev_, cameraCB_, sizeof(*mappedScene_));
	cameraCB_->Map(0, nullptr, (void**)&mappedScene_);
	mappedScene_->lightVec = XMFLOAT3(1.0f, -1.0f, 1.0f);

	CreateDescriptorHeap(&dev_, cameraHeap_);

	// 定数バッファビューの作成
	CreateConstantBufferView(&dev_, cameraCB_, cameraHeap_->GetCPUDescriptorHandleForHeapStart());

	return true;
}

void Camera::UpdateCamera()
{
	// カメラの更新
	auto wsize = Application::Instance().GetWindowSize();
	XMVECTOR eyePos = XMLoadFloat3(&eye_);
	XMVECTOR targetPos = XMLoadFloat3(&target_);
	XMVECTOR upVec = XMLoadFloat3(&up_);
	XMVECTOR lightVec = XMLoadFloat3(&mappedScene_->lightVec);
	lightVec = XMVector3Normalize(lightVec);

	auto cameraArmLength = XMVector3Length(XMVectorSubtract(targetPos, eyePos)).m128_f32[0];
	XMVECTOR lightCamPos = targetPos - lightVec * cameraArmLength;

	// カメラ用
	auto view = XMMatrixLookAtLH(eyePos, targetPos, upVec);
	auto proj = XMMatrixPerspectiveFovLH(
		fov_,
		static_cast<float>(wsize.w) / static_cast<float>(wsize.h),
		0.05f, 1000.0f);

	// ライト用
	auto lightView = XMMatrixLookAtLH(lightCamPos, targetPos, upVec);
	auto lightProj = XMMatrixOrthographicLH(80, 80, 0.05f, 1000.0f);

	mappedScene_->view = view;
	mappedScene_->proj = proj;
	mappedScene_->eye = eye_;
	mappedScene_->invProj = XMMatrixInverse(nullptr, proj);
	mappedScene_->lightCamera = lightView * lightProj;
}