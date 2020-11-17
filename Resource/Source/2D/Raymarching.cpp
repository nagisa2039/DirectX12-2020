#include "Raymarching.h"

Raymarching::Raymarching():
	Material(L"Resource/Source/Shader/2D/Raymarching.hlsl")
{
	CreateEachDataBuffer();
}

Raymarching::~Raymarching()
{
}