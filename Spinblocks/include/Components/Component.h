#pragma once

namespace Components
{
	class Component
	{
	private:
		bool m_enabled; // Is the component enabled to systems?

	public:
		Component(bool enabled = true) : m_enabled(enabled)
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