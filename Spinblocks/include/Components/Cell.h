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

		entt::entity GetNorth() const
		{
			return m_north;
		}
		entt::entity GetSouth() const
		{
			return m_south;
		}
		entt::entity GetWest() const
		{
			return m_west;
		}
		entt::entity GetEast() const
		{
			return m_east;
		}

		entt::entity GetParent() const
		{
			return m_parent;
		}

		entt::entity GetDirection(const moveDirection_t& direction) const
		{
			switch (direction)
			{
			case moveDirection_t::NORTH:
				return GetNorth();
			case moveDirection_t::EAST:
				return GetEast();
			case moveDirection_t::SOUTH:
				return GetSouth();
			case moveDirection_t::WEST:
				return GetWest();
			default:
				assert(false);
			}
		}
	};
}