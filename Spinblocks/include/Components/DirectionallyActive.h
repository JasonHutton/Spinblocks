#pragma once

#include "Components/Component.h"

enum class moveDirection_t;

namespace Components
{
	class DirectionallyActive : public Component
	{
	protected:
		moveDirection_t m_direction;

	public:
		DirectionallyActive(const moveDirection_t& direction) : m_direction(direction)
		{

		}

		const bool IsActive(const moveDirection_t& direction) const
		{
			if (direction == m_direction)
				return true;

			return false;
		}
	};
}