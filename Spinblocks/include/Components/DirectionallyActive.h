#pragma once

#include "Components/Component.h"

#include <vector>

enum class moveDirection_t;

namespace Components
{
	class DirectionallyActive : public Component
	{
	private:
		std::vector<moveDirection_t> m_directions;

	public:
		DirectionallyActive(const std::vector<moveDirection_t>& directions) : m_directions(directions)
		{

		}

		DirectionallyActive(const moveDirection_t& direction)
		{
			AddDirection(direction);
		}

		void AddDirection(const moveDirection_t& direction)
		{
			m_directions.push_back(direction);
		}

		const bool IsActive(const moveDirection_t& direction) const
		{
			for (const auto& dir : m_directions)
			{
				if (direction == dir)
					return true;
			}

			return false;
		}
	};
}