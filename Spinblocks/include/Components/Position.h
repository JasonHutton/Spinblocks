#pragma once

#include "Component.h"
#include "glm/vec3.hpp"

namespace Components
{
	class Position : public Component
	{
	private:
		bool m_enabled; // Is the component enabled to systems?
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
	public:
		void Enable(bool enable = true)
		{
			m_enabled = enable;
		}

		const bool& IsEnabled() const
		{
			return m_enabled;
		}
	};
}