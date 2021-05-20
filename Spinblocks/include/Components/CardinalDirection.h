#pragma once

#include "Components/Component.h"
#include "glm/glm.hpp"

enum class rotationDirection_t;
enum class moveDirection_t;

namespace Components
{
	class CardinalDirection : public Component
	{
	private:
		moveDirection_t m_currentOrientation{ moveDirection_t::NORTH };
		moveDirection_t m_desiredOrientation{ moveDirection_t::NORTH };

	public:
		CardinalDirection()
		{
		}

		const moveDirection_t& GetCurrentOrientation() const
		{
			return m_currentOrientation;
		}

		const moveDirection_t& GetDesiredOrientation() const
		{
			return m_desiredOrientation;
		}

		void SetCurrentOrientation(const moveDirection_t& moveDirection)
		{
			m_currentOrientation = moveDirection;
		}

		void SetDesiredOrientation(const moveDirection_t& moveDirection)
		{
			m_desiredOrientation = moveDirection;
		}

		const moveDirection_t GetCurrentDownDirection() const
		{
			switch (m_currentOrientation)
			{
			case moveDirection_t::NORTH:
				return moveDirection_t::SOUTH;
			case moveDirection_t::EAST:
				return moveDirection_t::EAST;
			case moveDirection_t::SOUTH:
				return moveDirection_t::NORTH;
			case moveDirection_t::WEST:
				return moveDirection_t::WEST;
			default:
				return moveDirection_t::SOUTH;
			}

			return moveDirection_t::SOUTH;
		}

		const moveDirection_t GetCurrentUpDirection() const
		{
			switch (m_currentOrientation)
			{
			case moveDirection_t::NORTH:
				return moveDirection_t::NORTH;
			case moveDirection_t::EAST:
				return moveDirection_t::WEST;
			case moveDirection_t::SOUTH:
				return moveDirection_t::SOUTH;
			case moveDirection_t::WEST:
				return moveDirection_t::EAST;
			default:
				return moveDirection_t::NORTH;
			}

			return moveDirection_t::NORTH;
		}

		const moveDirection_t GetCurrentLeftDirection() const
		{
			switch (m_currentOrientation)
			{
			case moveDirection_t::NORTH:
				return moveDirection_t::WEST;
			case moveDirection_t::EAST:
				return moveDirection_t::SOUTH;
			case moveDirection_t::SOUTH:
				return moveDirection_t::EAST;
			case moveDirection_t::WEST:
				return moveDirection_t::NORTH;
			default:
				return moveDirection_t::WEST;
			}

			return moveDirection_t::WEST;
		}

		const moveDirection_t GetCurrentRightDirection() const
		{
			switch (m_currentOrientation)
			{
			case moveDirection_t::NORTH:
				return moveDirection_t::EAST;
			case moveDirection_t::EAST:
				return moveDirection_t::NORTH;
			case moveDirection_t::SOUTH:
				return moveDirection_t::WEST;
			case moveDirection_t::WEST:
				return moveDirection_t::SOUTH;
			default:
				return moveDirection_t::EAST;
			}

			return moveDirection_t::EAST;
		}

		const float GetAngleInRadiansOfOrientation(const moveDirection_t& currentOrientation) const
		{
			switch (currentOrientation)
			{
			case moveDirection_t::NORTH:
				return glm::radians(0.0f);
			case moveDirection_t::EAST:
				return glm::radians(-90.0f);
			case moveDirection_t::SOUTH:
				return glm::radians(180.0f);
			case moveDirection_t::WEST:
				return glm::radians(90.0f);
			default:
				return glm::radians(0.0f);
			}

			return glm::radians(0.0f);
		}

		const moveDirection_t GetNewOrientation(const rotationDirection_t& rotationDirection, const moveDirection_t& currentOrientation) const
		{
			if (rotationDirection == rotationDirection_t::CLOCKWISE)
			{
				switch (currentOrientation)
				{
				case moveDirection_t::NORTH:
					return moveDirection_t::EAST;
				case moveDirection_t::EAST:
					return moveDirection_t::SOUTH;
				case moveDirection_t::SOUTH:
					return moveDirection_t::WEST;
				case moveDirection_t::WEST:
					return moveDirection_t::NORTH;
				default:
					assert(false);
				}
			}
			else if (rotationDirection == rotationDirection_t::COUNTERCLOCKWISE)
			{
				switch (currentOrientation)
				{
				case moveDirection_t::NORTH:
					return moveDirection_t::WEST;
				case moveDirection_t::WEST:
					return moveDirection_t::SOUTH;
				case moveDirection_t::SOUTH:
					return moveDirection_t::EAST;
				case moveDirection_t::EAST:
					return moveDirection_t::NORTH;
				default:
					assert(false);
				}
			}

			assert(false);
		}
	};
}