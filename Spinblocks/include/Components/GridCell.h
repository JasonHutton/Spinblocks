#pragma once

#include "Components/Component.h"
#include "glm/vec2.hpp"

namespace Components
{
	class GridCell : public Component
	{
	protected:
		glm::uvec2 m_dimensions;
	public:
		GridCell(const glm::uvec2& dimensions) : m_dimensions(dimensions)
		{

		}

		const glm::uvec2 GetDimensions() const
		{
			return m_dimensions;
		}
	};
}