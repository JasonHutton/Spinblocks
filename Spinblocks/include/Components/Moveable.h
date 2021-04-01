#pragma once

#include "Components/Component.h"
#include "Coordinate.h"

namespace Components
{
	enum class movementStates_t
	{
		UNMOVING,
		FOLLOWING,
		FALL,
		DEBUG_MOVE_UP,
		SOFT_DROP,
		HARD_DROP,
		LOCKED
	};

	class Moveable : public Component
	{
	protected:
		Coordinate m_desiredCoordinate;
		Coordinate m_currentCoordinate;

		movementStates_t m_movementState{ movementStates_t::UNMOVING };

	public:
		Moveable(const Coordinate& currentCoordinate, const Coordinate& desiredCoordiante) : m_currentCoordinate(currentCoordinate), m_desiredCoordinate(desiredCoordiante)
		{
		}

		void SetCurrentCoordinate(const Coordinate& currentCoordinate)
		{
			m_currentCoordinate = currentCoordinate;
		}

		const Coordinate& GetCurrentCoordinate() const
		{
			return m_currentCoordinate;
		}

		void SetDesiredCoordinate(const Coordinate& desiredCoordinate)
		{
			m_desiredCoordinate = desiredCoordinate;
		}

		const Coordinate& GetDesiredCoordinate() const
		{
			return m_desiredCoordinate;
		}

		const movementStates_t& GetMovementState() const
		{
			return m_movementState;
		}

		void SetMovementState(const movementStates_t& movementState)
		{
			m_movementState = movementState;
		}
	};
}