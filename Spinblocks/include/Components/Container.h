#pragma once

#include "Component.h"
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
		glm::vec2 m_paddingPercentage; // How much should the grid be padded within the container? TODO not implemented yet

		glm::uvec2 m_parentDimensions; // In screen coordinates, how much room do we have to work with?
		
		std::vector<entt::entity> m_grid;
		glm::vec2 m_gridSquareDimensions;
	public:
		Container(const glm::uvec2& gridDimensions, const glm::uvec2& parentDimensions, const glm::vec2& percentageOfDimensions, const glm::vec2& paddingPercentage = glm::vec2(1.0f, 1.0f)) : 
			m_gridDimensions(gridDimensions), m_parentDimensions(parentDimensions), m_percentageOfDimensions(percentageOfDimensions), m_paddingPercentage(paddingPercentage)
		{
			//for(int i = 0;i < m_gridDimensions.x * m_gridDimensions.y;i++)

			m_grid.reserve((size_t)m_gridDimensions.x * (size_t)m_gridDimensions.y);

			UpdateGridSquareDimensions();
		}

		// Just have a fixed target resolution to avoid floating point alignment issues?
		// 640 * 0.4 = 256 / 10 = 25.6...truncated to 25?
		// Do a floating point coordinate? Look into if that's possible.
		void UpdateGridSquareDimensions()
		{
			glm::uvec2 dim;

			dim.x = (glm::uint)((float)m_parentDimensions.x * m_percentageOfDimensions.x);
			dim.y = (glm::uint)((float)m_parentDimensions.y * m_percentageOfDimensions.y);

			dim.x /= m_gridDimensions.x;
			dim.y /= m_gridDimensions.y;

			m_gridSquareDimensions = dim;
		}

		const glm::vec2& GetGridDimensions() const
		{
			return m_gridDimensions;
		}

		const glm::vec2& GetPercentageOfDimensions() const
		{
			return m_percentageOfDimensions;
		}

		/*
	protected:
		glm::vec3 m_scale;

	public:
		Scale(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
		{
			m_scale = scale;
		}

		const glm::vec3& Get() const
		{
			return m_scale;
		}

		void Set(const glm::vec3& scale)
		{
			m_scale = scale;
		}*/
	};
}