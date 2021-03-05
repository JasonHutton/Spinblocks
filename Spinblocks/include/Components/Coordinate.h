#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include "glm/vec2.hpp"
#include <sstream>

namespace Components
{
	class Coordinate : public Component
	{
	protected:
		glm::uvec2 m_coordinate;

	public:
		Coordinate(const glm::uvec2& coordinate) : m_coordinate(coordinate)
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

	public:
		friend std::ostream& operator<<(std::ostream& out, const Coordinate& coord)
		{
			out << coord.m_coordinate.x << "," << coord.m_coordinate.y;
			return out;
		}

	public:
		friend bool operator==(const Coordinate& lhs, const Coordinate& rhs)
		{
			return glm::all(glm::equal(lhs.m_coordinate, rhs.m_coordinate));
		}
		friend bool operator!=(const Coordinate& lhs, const Coordinate& rhs)
		{
			return !glm::all(glm::equal(lhs.m_coordinate, rhs.m_coordinate));
		}
	public:
		operator glm::uvec2() const { return Get(); }

	};
}

std::string to_string(Components::Coordinate coord);