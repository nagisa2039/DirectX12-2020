#include "StanderedMaterial.h"

StanderedMaterial::StanderedMaterial()
	:Material(L"Resource/Source/Shader/2D/2DStanderdPS.hlsl")
{
	AddEachData();
	CreateEachDataBuffer();
}

StanderedMaterial::~StanderedMaterial()
{
}

void StanderedMaterial::AddEachData()
{
}