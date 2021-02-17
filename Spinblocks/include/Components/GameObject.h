#pragma once

#include "Component.h"

namespace Components
{
	class GameObject : public Component
	{
	private:
		bool m_enabled; // Is the component enabled to systems?

	public:
		GameObject()
		{
			Enable();
		}

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