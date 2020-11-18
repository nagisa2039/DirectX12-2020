#include "StanderedMaterial.h"

StanderedMaterial::StanderedMaterial(const std::wstring& shaderPath)
	:Material(shaderPath)
{
	CreateEachDataBuffer();
}

StanderedMaterial::~StanderedMaterial()
{
}