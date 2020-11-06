#include "PlaneMesh.h"
#include <array>
#include "System/Dx12Wrapper.h"

using namespace std;
using namespace DirectX;

PlaneMesh::PlaneMesh(Dx12Wrapper& dx12, const XMFLOAT3& pos, const float width, const float depth, std::wstring texPath)
	:PrimitiveMesh(dx12, pos, texPath), _width(width), _depth(depth)
{
	vector<PrimVertex> vertices
	{
		{ XMFLOAT3(-_width / 2, 0, -_depth / 2), XMFLOAT3(0,1,0) , XMFLOAT2(0,1)},		// ���O
		{ XMFLOAT3(-_width / 2, 0, +_depth / 2), XMFLOAT3(0,1,0) , XMFLOAT2(0,0) },	// ����
		{ XMFLOAT3(+_width / 2, 0, -_depth / 2), XMFLOAT3(0,1,0) , XMFLOAT2(1,1) },	// �E�O
		{ XMFLOAT3(+_width / 2, 0, +_depth / 2), XMFLOAT3(0,1,0) , XMFLOAT2(1,0) }		// �E��
	};

	vector<uint16_t> indices = { 0,1,2, 2,1,3 };

	CalNormalVertex(vertices, indices);
	CreateVertexBufferAndView(vertices);
	CreateIndexBufferAndView(indices);

	_rotate = { 0.0f,0.0f,0.0f };
	CreateTransBuffer();
}

PlaneMesh::~PlaneMesh()
{
}