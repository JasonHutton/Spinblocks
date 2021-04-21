#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class ProjectionOf : public ReferenceEntity
	{
	public:
		ProjectionOf(entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}