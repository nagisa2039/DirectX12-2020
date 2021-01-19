#pragma once
#include "imgui/imgui.h"
#include <DirectXMath.h>

namespace
{
	// DrawFloat3の描画(ラベル名, XMFLOAT3の操作対象, 速度, 最小値, 最大値)
	void DragXMFLOAT3(const char* label, DirectX::XMFLOAT3& inVec, float speed = 1.0f, float min = 0.0f, float max = 1.0f)
	{
		float f[3] = { inVec.x, inVec.y, inVec.z };
		ImGui::DragFloat3(label, f, speed, min, max);
		inVec = DirectX::XMFLOAT3(f[0], f[1], f[2]);
	}

	// SliderFloat3の描画(ラベル名, XMFLOAT3の操作対象, 最小値, 最大値)
	void SliderXMFLOAT3(const char* label, DirectX::XMFLOAT3& inVec, float min = 0.0f, float max = 1.0f)
	{
		float f[3] = { inVec.x, inVec.y, inVec.z };
		ImGui::SliderFloat3(label, f, min, max);
		inVec = DirectX::XMFLOAT3(f[0], f[1], f[2]);
	}

	// ColorPicker4の描画(ラベル名, XMFLOAT4の操作対象)
	void PickColorXMFLOAT4(const char* label, DirectX::XMFLOAT4& inVec)
	{
		ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode(label))
		{
			float f[4] = { inVec.x, inVec.y, inVec.z ,inVec.w };
			ImGui::ColorPicker4("Color", f);
			inVec = DirectX::XMFLOAT4(f[0], f[1], f[2], f[3]);

			ImGui::TreePop();
		}
	}

	// ColorPicker3の描画(ラベル名, XMFLOAT3の操作対象)
	void PickColorXMFLOAT3(const char* label, DirectX::XMFLOAT3& inVec)
	{
		ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode(label))
		{
			float f[] = { inVec.x, inVec.y, inVec.z };
			ImGui::ColorPicker3("Color", f);
			inVec = DirectX::XMFLOAT3(f[0], f[1], f[2]);

			ImGui::TreePop();
		}
	}
}