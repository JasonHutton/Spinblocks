#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class ScaleToCellDimensions : public ReferenceEntity
	{
	public:
		ScaleToCellDimensions(entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}