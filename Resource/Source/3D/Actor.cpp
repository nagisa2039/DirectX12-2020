#include "Actor.h"
#include "Utility/dx12Tool.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"
#include "3D/Component.h"

using namespace DirectX;

Actor::Actor()
{
	// 座標の定数バッファの作成
	auto& dev = Application::Instance().GetDx12().GetDevice();
	CreateUploadBuffer(&dev, transCB_, sizeof(*mappedTrans_));
	transCB_->Map(0, nullptr, (void**)&mappedTrans_);

	// 座標のヒープ作成
	CreateDescriptorHeap(&dev, transHeap_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

	CreateConstantBufferView(&dev, transCB_, transHeap_->GetCPUDescriptorHandleForHeapStart());

	trans_.pos		= { 0.0f, 0.0f, 0.0f };
	trans_.rotate	= { 0.0f, 0.0f, 0.0f };
	trans_.scale	= { 1.0f, 1.0f, 1.0f };
	UpdateTransform();
}

Actor::~Actor()
{
	int a = 0;
}

void Actor::Update()
{
	for (auto& component : components_)
	{
		component->Update();
	}
}

const Transform& Actor::GetTransform() const
{
	return trans_;
}

void Actor::SetTransform(const Transform& transform)
{
	trans_ = transform;

	UpdateTransform();
}

void Actor::UpdateTransform()
{
	const float deg2rad = (XM_PI / 180.0f);
	*mappedTrans_ =
		XMMatrixRotationRollPitchYaw(trans_.rotate.x * deg2rad, trans_.rotate.y * deg2rad, trans_.rotate.z * deg2rad)
		* XMMatrixScaling(trans_.scale.x, trans_.scale.y, trans_.scale.z)
		* XMMatrixTranslation(trans_.pos.x, trans_.pos.y, trans_.pos.z);
}

void Actor::SetTransformHeap(const UINT rootParamatorIndex)
{
	auto& commandList = Application::Instance().GetDx12().GetCommand().CommandList();
	commandList.SetDescriptorHeaps(1, transHeap_.GetAddressOf());
	commandList.SetGraphicsRootDescriptorTable(7, transHeap_->GetGPUDescriptorHandleForHeapStart());
}

void Actor::AddComponent(std::shared_ptr<Component> component)
{
	components_.emplace_back(component);
}

const ComPtr<ID3D12DescriptorHeap>& Actor::GetTransformHeap() const
{
	return transHeap_;
}
