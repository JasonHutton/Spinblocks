#pragma once

namespace Components
{
	class Component
	{
	public:
		void Enable(bool enable = true);
		const bool& IsEnabled() const;
	};
}