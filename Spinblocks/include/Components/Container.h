#pragma once

#include "Components/Component.h"
#include "GridCell.h"
#include "glm/vec2.hpp"
#include <entt/entity/registry.hpp>
#include <vector>

namespace Components
{
	class Container : public Component
	{
	protected:
		glm::uvec2 m_gridDimensions; // In grid coordinates. We work out screen coordinates from this and the other values.
		
		// These should be normalized values between 1.0 and 0.0
		glm::vec2 m_percentageOfDimensions; // How much space from the containing component are we consuming in each direction?
		//glm::vec2 m_paddingPercentage; // How much should the grid be padded within the container? TODO not implemented yet

		//glm::uvec2 m_parentDimensions; // In screen coordinates, how much room do we have to work with?
	public:
		std::vector<entt::entity> m_grid;
	protected:
		glm::vec2 m_gridSquareDimensions;
	public:
		Container(const glm::vec2& percentageOfDimensions, const glm::uvec2& gridDimensions, const glm::uvec2& gridSquareDimensions) :
			m_percentageOfDimensions(percentageOfDimensions), m_gridDimensions(gridDimensions), m_gridSquareDimensions(gridSquareDimensions)
		{
			//for(int i = 0;i < m_gridDimensions.x * m_gridDimensions.y;i++)

			m_grid.reserve((size_t)m_gridDimensions.x * (size_t)m_gridDimensions.y);
		}

		const glm::vec2& GetGridDimensions() const
		{
			return m_gridDimensions;
		}

		const glm::vec2& GetPercentageOfDimensions() const
		{
			return m_percentageOfDimensions;
		}

		// Gets coordinates relative to the parent(this) position
		const glm::vec2& GetGridSquareCoordinates(const glm::uvec2& gridPos) const
		{
			int x = 0;
			int y = 0;

			x = -(m_gridDimensions.x/2 * m_gridSquareDimensions.x);
			x += gridPos.x * m_gridSquareDimensions.x;

			y = -(m_gridDimensions.y / 2 * m_gridSquareDimensions.y);
			y += gridPos.y * m_gridSquareDimensions.y;

			return glm::vec2(x, y);
		}

		const glm::uvec2 GetGridSquareDimensions(const entt::registry& registry, const entt::entity& entity)
		{
			auto& gridCell = registry.get<Components::GridCell>(entity);
			return gridCell.GetDimensions();
		}

		entt::entity GetGridCell(const entt::registry& registry, const entt::entity& entity, const glm::uvec2 pos)
		{

		}
	};
}