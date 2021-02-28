#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class DerivePositionFromParent : public ReferenceEntity
	{
	public:
		DerivePositionFromParent(entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}