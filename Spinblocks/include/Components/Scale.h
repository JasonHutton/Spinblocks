#pragma once

#include "Component.h"
#include "glm/vec3.hpp"

namespace Components
{
	class Scale : public Component
	{
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
		}
	};
}