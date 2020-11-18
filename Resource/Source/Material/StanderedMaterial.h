#pragma once
#include "Material.h"
class StanderedMaterial :
    public Material
{
public:
	StanderedMaterial(const std::wstring& shaderPath);
	~StanderedMaterial();
};

