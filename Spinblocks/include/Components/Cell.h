#pragma once

#include "Component.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Cell : public Component
	{
	protected:
		entt::entity m_parent { entt::null };
		entt::entity m_up { entt::null };
		entt::entity m_down { entt::null };
		entt::entity m_left { entt::null };
		entt::entity m_right { entt::null };
		
	public:
		Cell(entt::entity parent) : m_parent(parent)
		{
		}

		void SetUp(entt::entity ent)
		{
			m_up = ent;
		}
		void SetDown(entt::entity ent)
		{
			m_down = ent;
		}
		void SetLeft(entt::entity ent)
		{
			m_left = ent;
		}
		void SetRight(entt::entity ent)
		{
			m_right = ent;
		}

		entt::entity GetUp()
		{
			return m_up;
		}
		entt::entity GetDown()
		{
			return m_down;
		}
		entt::entity GetLeft()
		{
			return m_left;
		}
		entt::entity GetRight()
		{
			return m_right;
		}

		entt::entity GetParent()
		{
			return m_parent;
		}
	};
}