#pragma once

namespace Components
{
	class Component
	{
	protected:
		bool m_enabled; // Is the component enabled to systems?

	public:
		Component() : m_enabled(true)
		{

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