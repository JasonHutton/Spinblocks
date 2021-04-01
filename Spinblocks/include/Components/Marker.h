#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Marker : public ReferenceEntity
	{
	public:
		Marker (entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}