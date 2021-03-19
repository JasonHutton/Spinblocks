#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>
#include "Utility.h"

enum class moveDirection_t;

namespace Components
{
	class CellLink : public Component
	{
	protected:
		entt::entity m_source{ entt::null };
		entt::entity m_destination{ entt::null };
		moveDirection_t m_direction;

	public:
		CellLink (entt::entity source, entt::entity destination, const moveDirection_t& direction) : m_source(source), m_destination(destination)
		{
			SetDirection(direction);
		}

		const entt::entity& GetSource() const
		{
			return m_source;
		}

		void SetSource(const entt::entity source)
		{
			m_source = source;
		}

		const entt::entity& GetDestination() const
		{
			return m_destination;
		}

		void SetAdjacent(const entt::entity destination)
		{
			m_destination = destination;
		}

		const moveDirection_t& GetDirection() const
		{
			return m_direction;
		}

		void SetDirection(const moveDirection_t& direction)
		{
			m_direction = direction;
		}
	};
}