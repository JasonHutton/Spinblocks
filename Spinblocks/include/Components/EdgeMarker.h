#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>
#include "Utility.h"

enum class moveDirection_t;

namespace Components
{
	class EdgeMarker : public ReferenceEntity
	{
	protected:
		entt::entity m_adjacentEntity{ entt::null };
		moveDirection_t m_dir;

	public:
		EdgeMarker (entt::entity entity, entt::entity adjacentEntity, const moveDirection_t& dir) : ReferenceEntity(entity), m_adjacentEntity(adjacentEntity)
		{
			SetDirection(dir);
		}

		const entt::entity& GetAdjacent() const
		{
			return m_adjacentEntity;
		}

		void SetAdjacent(const entt::entity adjacentEntity)
		{
			m_adjacentEntity = adjacentEntity;
		}

		const moveDirection_t& GetDirection() const
		{
			return m_dir;
		}

		void SetDirection(const moveDirection_t& dir)
		{
			m_dir = dir;
		}
	};
}