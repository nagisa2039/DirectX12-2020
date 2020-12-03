#include "Component.h"

Component::Component(std::weak_ptr<Actor> owner)
	:owner_(owner)
{
}

std::weak_ptr<Actor> Component::GetOwner()const
{
	return owner_;
}