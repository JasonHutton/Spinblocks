#pragma once

#include "Components/Component.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace Components
{
	class Orientation : public Component
	{
	protected:
		float m_orientation;
		glm::vec3 m_axis;

	public:
		Orientation(const float& orientation, const glm::vec3& axis) : m_orientation(orientation), m_axis(axis)
		{
		}

		const float& Get() const
		{
			return m_orientation;
		}

		void Set(const float& orientation)
		{
			m_orientation = orientation;
		}

		const glm::vec3& GetAxis() const
		{
			return m_axis;
		}

		void SetAxis(const glm::vec3& axis)
		{
			m_axis = axis;
		}
	};
}