#pragma once

#include "Component.h"
#include "glm/vec3.hpp"

namespace Components
{
	class Position : public Component
	{
	protected:
		glm::vec3 m_position;

	public:
		Position(const glm::vec3& position)
		{
			m_position = position;
		}

		const glm::vec3& Get() const
		{
			return m_position;
		}

		void Set(const glm::vec3& position)
		{
			m_position = position;
		}
	};
}