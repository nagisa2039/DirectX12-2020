#include "PlaneMesh.h"
#include <array>
#include "System/Dx12Wrapper.h"
#include "Material/ModelMaterial.h"

using namespace std;
using namespace DirectX;

PlaneMesh::PlaneMesh(Dx12Wrapper& dx12, const XMFLOAT3& pos, const float width, const float depth, std::wstring texPath)
	:PrimitiveMesh(dx12, pos, texPath), width_(width), depth_(depth)
{
	vector<PrimVertex> vertices
	{
		{ XMFLOAT3(-width_ / 2, 0, -depth_ / 2), XMFLOAT3(0,1,0) , XMFLOAT2(0,1)},		// 左前
		{ XMFLOAT3(-width_ / 2, 0, +depth_ / 2), XMFLOAT3(0,1,0) , XMFLOAT2(0,0) },	// 左後
		{ XMFLOAT3(+width_ / 2, 0, -depth_ / 2), XMFLOAT3(0,1,0) , XMFLOAT2(1,1) },	// 右前
		{ XMFLOAT3(+width_ / 2, 0, +depth_ / 2), XMFLOAT3(0,1,0) , XMFLOAT2(1,0) }		// 右後
	};

	vector<uint16_t> indices = { 0,1,2, 2,1,3 };

	CalNormalVertex(vertices, indices);
	CreateVertexBufferAndView(vertices);
	CreateIndexBufferAndView(indices);

	rotate_ = { 0.0f,0.0f,0.0f };
	CreateTransBuffer();

	std::vector<MaterialBase> mbvec;
	mbvec.emplace_back(MaterialBase{
		XMFLOAT3(1.0f, 1.0f, 1.0f), 
		XMFLOAT3(1.0f, 1.0f, 1.0f), 
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		0.0f, -1 });
	std::vector<int> addTexVec(1);
	material_ = make_unique<ModelMaterial>(mbvec, addTexVec);
}

PlaneMesh::~PlaneMesh()
{
}