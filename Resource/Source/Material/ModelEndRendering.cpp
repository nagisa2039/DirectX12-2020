#include "ModelEndRendering.h"
#include "3D/RendererManager.h"
#include "Utility/Cast.h"
#include "Utility/Constant.h"
#include "System/Application.h"
#include "System/Dx12Wrapper.h"
#include "System/TexLoader.h"

ModelEndRendering::ModelEndRendering()
	: Material(L"Resource/Source/Shader/2D/ModelEndRendering.hlsl")
{
	auto& texLoader = Application::Instance().GetDx12().GetTexLoader();
	std::wstring screenNames[] = {
		SCR_CAMERA_MR_COLOR, SCR_CAMERA_MR_NORMAL , SCR_CAMERA_MR_BRIGHT, 
		SCR_COLOR_SHRINK, SCR_EMMISION_SHRINK };

	addTexIndexResource_.elements.reserve(Uint64(RendererManager::RenderTargetType::max));
	for (const auto& name : screenNames)
	{
		addTexIndexResource_.elements.emplace_back(texLoader.GetGraphHandle(name));
	}
	CreateEachDataBuffer();
}

ModelEndRendering::~ModelEndRendering()
{
}