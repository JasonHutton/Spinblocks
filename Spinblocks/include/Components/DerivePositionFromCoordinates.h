#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class DerivePositionFromCoordinates : public ReferenceEntity
	{
	public:
		DerivePositionFromCoordinates(entt::entity entity = entt::null) : ReferenceEntity(entity)
		{
		}
	};
}