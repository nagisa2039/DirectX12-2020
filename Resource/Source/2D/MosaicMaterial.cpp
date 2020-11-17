#include "MosaicMaterial.h"

MosaicMaterial::MosaicMaterial() :
	Material(L"Resource/Source/Shader/2D/Mosaic.hlsl")
{
	CreateEachDataBuffer();
}

MosaicMaterial::~MosaicMaterial()
{
}