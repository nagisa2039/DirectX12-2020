#include "Mesh.h"

Mesh::Mesh(const Mesh::Type type, std::shared_ptr<Actor> owner) :Component(owner),type_(type)
{
}

Mesh::Type Mesh::GetMeshType() const
{
    return type_;
}