#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Block : public ReferenceEntity
	{
	public:
		Block (entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}