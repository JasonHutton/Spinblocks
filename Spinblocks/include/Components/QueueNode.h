#pragma once

#include "Components/Component.h"
#include "Components/CellLink.h"
#include <entt/entity/registry.hpp>

enum class moveDirection_t;

namespace Components
{
	class QueueNode : public CellLink
	{
	protected:
		entt::entity m_contentEntity{ entt::null };
		entt::entity m_self{ entt::null };

	public:
		QueueNode (entt::entity self, entt::entity source = entt::null, entt::entity destination = entt::null) : m_self(self), CellLink(source, destination, moveDirection_t::NORTH)
		{
		}

		entt::entity GetSelf() const
		{
			return m_self;
		}

		entt::entity GetContent() const
		{
			return m_contentEntity;
		}

		void SetContent(const entt::entity& content)
		{
			m_contentEntity = content;
		}

		entt::entity GetDestination() const
		{
			return m_destination;
		}

		void SetDestination(const entt::entity destination)
		{
			m_destination = destination;
		}

	private:
		void SetAdjacent(const entt::entity destination) = delete;
		const moveDirection_t& GetDirection() const = delete;
		void SetDirection(const moveDirection_t& direction) = delete;
	};
}