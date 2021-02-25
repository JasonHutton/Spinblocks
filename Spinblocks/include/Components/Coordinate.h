#pragma once

#include "Component.h"
#include "glm/vec2.hpp"
#include <ostream>

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
		friend std::ostream& operator<<(std::ostream& out, const Coordinate& coord);
	};
}