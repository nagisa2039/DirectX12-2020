#include "Actor.h"
#include "Utility/dx12Tool.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"
#include "3D/Component.h"
#include "Utility/ImGuiTool.h"
#include <sstream>

using namespace DirectX;

namespace
{
	constexpr XMVECTOR FORWERD_VEC	= { 0.0f, 0.0f, 1.0f };
	constexpr XMVECTOR UP_VEC		= { 0.0f, 1.0f, 0.0f };
	constexpr XMVECTOR RIGHT_VEC	= { 1.0f, 0.0f, 0.0f };
}

Actor::Actor()
{
	// 座標の定数バッファの作成
	auto& dev = Application::Instance().GetDx12().GetDevice();
	CreateConstantBufferAndHeap(&dev, mappedTrans_, transCB_, transHeap_, sizeof(Transform));

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
	UpdateTransform();
}

const std::string& Actor::GetName() const
{
	return name_;
}

void Actor::SetName(const std::string& n)
{
	name_ = n;
}

Transform& Actor::GetTransform()
{
	return trans_;
}

void Actor::SetTransform(const Transform& transform)
{
	trans_ = transform;
}

void Actor::UpdateTransform()
{
	*mappedTrans_ = trans_.GetMatrix();
}

void Actor::SetTransformHeap(const UINT rootParamatorIndex, const bool compute)
{
	auto& commandList = Application::Instance().GetDx12().GetCommand().CommandList();
	commandList.SetDescriptorHeaps(1, transHeap_.GetAddressOf());
	if (compute)
	{
		commandList.SetComputeRootDescriptorTable(rootParamatorIndex,
			transHeap_->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		commandList.SetGraphicsRootDescriptorTable(rootParamatorIndex, 
			transHeap_->GetGPUDescriptorHandleForHeapStart());
	}
}

void Actor::AddComponent(std::shared_ptr<Component> component)
{
	component->Init();
	components_.emplace_back(component);
}

void Actor::DrawImGui(const int num)
{
	std::stringstream ss;
	ss << name_ << num;
	if (ImGui::TreeNode(ss.str().c_str()))
	{
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Transform"))
		{
			DragXMFLOAT3("Position", trans_.pos, 0.1f, -100, 100);
			DragXMFLOAT3("Rotation", trans_.rotate, 1.0f, -360, 360);
			DragXMFLOAT3("Scale", trans_.scale, 0.1f, -100, 100);
			ImGui::TreePop();
		}

		for (auto& cmp : components_)
		{
			cmp->DrawImGui();
		}

		ImGui::TreePop();
	}
}

const ComPtr<ID3D12DescriptorHeap>& Actor::GetTransformHeap() const
{
	return transHeap_;
}

DirectX::XMMATRIX Transform::GetMatrix() const
{
	return GetRotateMatrix()
		* XMMatrixScaling(scale.x, scale.y, scale.z)
		* XMMatrixTranslation(pos.x, pos.y, pos.z);
}

DirectX::XMMATRIX Transform::GetRotateMatrix() const
{
	const float deg2rad = (XM_PI / 180.0f);
	return XMMatrixRotationRollPitchYaw(rotate.x * deg2rad, rotate.y * deg2rad, rotate.z * deg2rad);
}

DirectX::XMVECTOR Transform::GetForwerd() const
{
	return XMVector3Transform(FORWERD_VEC, GetRotateMatrix());
}

DirectX::XMVECTOR Transform::GetUp() const
{
	return XMVector3Transform(UP_VEC, GetRotateMatrix());
}

DirectX::XMVECTOR Transform::GetRight() const
{
	return XMVector3Transform(RIGHT_VEC, GetRotateMatrix());
}
