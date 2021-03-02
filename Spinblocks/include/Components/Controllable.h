#pragma once

#include "Component.h"

namespace Components
{
	class Controllable : public Component
	{
	protected:
		glm::uvec2 m_coordinate;

	public:
		Controllable()
		{
		}
	};
}