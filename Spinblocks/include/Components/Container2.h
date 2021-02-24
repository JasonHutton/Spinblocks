#pragma once

#include "Component.h"

namespace Components
{
	class Container2 : public Component
	{
	protected:
		glm::uvec2 m_gridDimensions; // In grid coordinates, not screen position.
		glm::uvec2 m_cellDimensions; // In screen coordinates
	
	public:
		Container2(const glm::uvec2& gridDimensions, const glm::uvec2& cellDimensions) : m_gridDimensions(gridDimensions), m_cellDimensions(cellDimensions)
		{

		}

		const glm::vec2& GetGridDimensions() const
		{
			return m_gridDimensions;
		}

		const glm::vec2& GetCellDimensions() const
		{
			return m_cellDimensions;
		}
	};

	
}