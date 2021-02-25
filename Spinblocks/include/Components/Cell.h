#pragma once

#include "Component.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Cell : public Component
	{
	protected:
		entt::entity m_parent { entt::null };
		entt::entity m_north { entt::null };
		entt::entity m_south { entt::null };
		entt::entity m_west { entt::null };
		entt::entity m_east { entt::null };
		
	public:
		Cell(entt::entity parent) : m_parent(parent)
		{
		}

		void SetNorth(entt::entity ent)
		{
			m_north = ent;
		}
		void SetSouth(entt::entity ent)
		{
			m_south = ent;
		}
		void SetWest(entt::entity ent)
		{
			m_west = ent;
		}
		void SetEast(entt::entity ent)
		{
			m_east = ent;
		}

		entt::entity GetNorth()
		{
			return m_north;
		}
		entt::entity GetSouth()
		{
			return m_south;
		}
		entt::entity GetWest()
		{
			return m_west;
		}
		entt::entity GetEast()
		{
			return m_east;
		}

		entt::entity GetParent()
		{
			return m_parent;
		}
	};
}