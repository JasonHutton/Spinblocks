#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Block : public ReferenceEntity
	{
	protected:

	public:
		Block (entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}