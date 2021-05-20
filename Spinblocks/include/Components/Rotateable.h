#pragma once

#include "Components/Component.h"

namespace Components
{
	class Rotateable : public Component
	{
	private:
		float m_desiredAngleInRadians;
		float m_currentAngleInRadians;

	public:
		Rotateable(const float& currentAngleInRadians, const float& desiredAngleInRadians) : m_currentAngleInRadians(currentAngleInRadians), m_desiredAngleInRadians(desiredAngleInRadians)
		{
		}

		void SetCurrentAngleInRadians(const float& currentAngleInRadians)
		{
			m_currentAngleInRadians = currentAngleInRadians;
		}

		const float& GetCurrentAngleInRadians() const
		{
			return m_currentAngleInRadians;
		}

		void SetDesiredAngleInRadians(const float& desiredAngleInRadians)
		{
			m_desiredAngleInRadians = desiredAngleInRadians;
		}

		const float& GetDesiredAngleInRadians() const
		{
			return m_desiredAngleInRadians;
		}
	};
}