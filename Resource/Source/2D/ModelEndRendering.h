#pragma once
#include "Material.h"

class ModelEndRendering :
	public Material
{
public:
	ModelEndRendering();
	~ModelEndRendering();

private:
	// Material を介して継承されました
	void AddEachData();
};

