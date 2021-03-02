#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Controllable : public ReferenceEntity
	{
	protected:

	public:
		Controllable(entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}