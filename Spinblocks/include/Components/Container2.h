#pragma once

#include "Component.h"

namespace Components
{
	class Container2 : public Component
	{
	protected:
		glm::uvec2 m_gridDimensions; // In grid coordinates, not screen position.
		glm::vec2 m_cellDimensions; // In screen coordinates
	
	public:
		Container2(const glm::uvec2& gridDimensions, const glm::vec2& cellDimensions) : m_gridDimensions(gridDimensions), m_cellDimensions(cellDimensions)
		{

		}

		const glm::uvec2& GetGridDimensions() const
		{
			return m_gridDimensions;
		}

		const glm::vec2& GetCellDimensions() const
		{
			return m_cellDimensions;
		}

		const glm::vec3 GetCellDimensions3() const
		{
			glm::vec2 dim = GetCellDimensions();
			return glm::vec3(dim.x, dim.y, 1.0f);
		}

		const glm::vec2 GetCellPosition(const glm::vec3& parentPosition, const glm::uvec2& coordinates) const
		{
			int x = 0;
			int y = 0;

			x = -(m_gridDimensions.x / 2 * m_cellDimensions.x);
			x += coordinates.x * m_cellDimensions.x;

			y = -(m_gridDimensions.y / 2 * m_cellDimensions.y);
			y += coordinates.y * m_cellDimensions.y;

			return glm::vec2(parentPosition.x + x, parentPosition.y + y);
		}

		const glm::vec3 GetCellPosition3(const glm::vec3& parentPosition, const glm::uvec2& coordinates) const
		{
			glm::vec2 pos = GetCellPosition(parentPosition, coordinates);
			return glm::vec3(pos.x, pos.y, parentPosition.z);
		}
	};

	
}