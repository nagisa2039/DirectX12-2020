#pragma once
#include "Material.h"
class StanderedMaterial :
    public Material
{
public:
	StanderedMaterial();
	~StanderedMaterial();

private:
	// Material ����Čp������܂���
	void AddEachData();
};

