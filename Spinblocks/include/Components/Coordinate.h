#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include "glm/vec2.hpp"
#include <sstream>
#include <entt/entity/registry.hpp>

namespace Components
{
	class Coordinate : public Component
	{
	protected:
		glm::uvec2 m_coordinate;
		entt::entity m_parent{ entt::null };

	public:
		Coordinate(const entt::entity& parent, const glm::uvec2& coordinate) : m_parent(parent), m_coordinate(coordinate)
		{
		}

		const glm::uvec2& Get() const
		{
			return m_coordinate;
		}

		void Set(const glm::uvec2& coordinate)
		{
			m_coordinate = coordinate;
		}

		const entt::entity& GetParent() const
		{
			return m_parent;
		}

		void SetParent(const entt::entity& parent)
		{
			m_parent = parent;
		}

	public:
		friend std::ostream& operator<<(std::ostream& out, const Coordinate& coord)
		{
			out << static_cast<entt::id_type>(coord.m_parent) << ":(" << coord.m_coordinate.x << "," << coord.m_coordinate.y << ")";
			return out;
		}

	public:
		friend bool operator==(const Coordinate& lhs, const Coordinate& rhs)
		{
			return (lhs.GetParent() == rhs.GetParent()) && glm::all(glm::equal(lhs.m_coordinate, rhs.m_coordinate));
		}
		friend bool operator!=(const Coordinate& lhs, const Coordinate& rhs)
		{
			return (lhs.GetParent() != rhs.GetParent()) || !glm::all(glm::equal(lhs.m_coordinate, rhs.m_coordinate));
		}
	public:
		// This isn't suitable for using as a comparison, as with set.insert(), as it doesn't fully compare things. Need to doublecheck if that can be corrected without breaking things.
		// GLM does have a less than comparison function...
		friend bool operator<(const Coordinate& lhs, const Coordinate& rhs)
		{
			if (lhs.GetParent() < rhs.GetParent())
				return true;

			if (lhs.GetParent() == rhs.GetParent())
			{
				if (lhs.m_coordinate.y < rhs.m_coordinate.y)
					return true;

				if (lhs.m_coordinate.y == rhs.m_coordinate.y)
				{
					if (lhs.m_coordinate.x < lhs.m_coordinate.x)
						return true;
				}
			}

			return false;
		}
		friend bool operator>(const Coordinate& lhs, const Coordinate& rhs) { return rhs < lhs; }
		friend bool operator<=(const Coordinate& lhs, const Coordinate& rhs) { return !(lhs > rhs); }
		friend bool operator>=(const Coordinate& lhs, const Coordinate& rhs) { return !(lhs < rhs); }
	//public:
		//operator glm::uvec2() const { return Get(); }

	};
}

std::string to_string(Components::Coordinate coord);