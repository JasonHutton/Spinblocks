#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Follower : public ReferenceEntity
	{
	public:
		Follower(entt::entity entity = entt::null) : ReferenceEntity(entity)
		{
		}
	};
}