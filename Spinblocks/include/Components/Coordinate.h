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
		operator glm::uvec2() const { return Get(); }

	};
}

std::string to_string(Components::Coordinate coord);