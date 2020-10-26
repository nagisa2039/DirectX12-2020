#pragma once
#include <d3d12.h>
#include "Utility/ComPtr.h"

class Command;

struct Resource
{
	ComPtr<ID3D12Resource> buffer;
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	bool Barrier(Command& cmd, const D3D12_RESOURCE_STATES changeState);
};