#pragma once
#include "Material.h"
class StanderedMaterial :
    public Material
{
public:
	StanderedMaterial();
	~StanderedMaterial();

private:
	// Material を介して継承されました
	void AddEachData();
};

