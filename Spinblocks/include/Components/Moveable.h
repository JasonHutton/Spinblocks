#pragma once

#include "Component.h"
#include "Coordinate.h"

namespace Components
{
	class Moveable : public Component
	{
	protected:
		Coordinate m_desiredCoordinate;
		Coordinate m_currentCoordinate;

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
	};
}