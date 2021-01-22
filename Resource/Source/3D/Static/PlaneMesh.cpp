#include "PlaneMesh.h"
#include <array>
#include "System/Dx12Wrapper.h"
#include "Material/ModelMaterial.h"
#include "System/TexLoader.h"
#include "3D/Actor.h"
#include "Utility/Constant.h"

using namespace std;
using namespace DirectX;

PlaneMesh::PlaneMesh(std::weak_ptr<Actor>owner,
	Dx12Wrapper& dx12, const XMFLOAT3& pos, const float width, const float depth, std::wstring texPath)
	:StaticMesh(owner, dx12, pos, texPath)
{
	vector<PrimVertex> vertices
	{
		{ XMFLOAT3(-0.5f, 0, -0.5f), XMFLOAT3(0,1,0) , XMFLOAT2(0,1)},		// 左前
		{ XMFLOAT3(-0.5f, 0, +0.5f), XMFLOAT3(0,1,0) , XMFLOAT2(0,0) },	// 左後
		{ XMFLOAT3(+0.5f, 0, -0.5f), XMFLOAT3(0,1,0) , XMFLOAT2(1,1) },	// 右前
		{ XMFLOAT3(+0.5f, 0, +0.5f), XMFLOAT3(0,1,0) , XMFLOAT2(1,0) }		// 右後
	};

	vector<uint16_t> indices = { 0,1,2, 2,1,3 };

	CalNormalVertex(vertices, indices);
	CreateVertexBufferAndView(vertices);
	CreateIndexBufferAndView(indices);

	std::vector<MaterialBase> mbvec;
	auto& texLoader = dx12_.GetTexLoader();
	auto texH = texLoader.LoadGraph(texPath);
	if (texH == FAILED)
	{
		texH = texLoader.GetDummyTextureHandles().whiteTexH;
	}
	mbvec.emplace_back(MaterialBase{
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
		XMFLOAT3(1.0f, 1.0f, 1.0f), 
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		0.0f, texH });
	std::vector<int> addTexVec(1);
	material_ = make_unique<ModelMaterial>(mbvec, addTexVec);

	GetOwner().lock()->GetTransform().scale = {width, 1.0f, depth};
}

PlaneMesh::~PlaneMesh()
{
}