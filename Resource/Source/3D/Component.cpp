#include "Component.h"

Component::Component(std::shared_ptr<Actor> owner)
	:owner_(owner)
{
}

std::weak_ptr<Actor> Component::GetOwner()
{
	return owner_;
}