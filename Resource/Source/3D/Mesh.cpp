#include "Mesh.h"
#include "System/Dx12Wrapper.h"
#include "3D/RendererManager.h"

Mesh::Mesh(Dx12Wrapper& dx12, std::weak_ptr<Actor>owner, const Mesh::Type type) 
    :Component(owner),type_(type), dx12_(dx12)
{
}

Mesh::~Mesh()
{
    dx12_.GetRendererManager().RemoveMesh(this);
}

void Mesh::Init()
{
    dx12_.GetRendererManager().AddMesh(this);
}

Mesh::Type Mesh::GetMeshType() const
{
    return type_;
}