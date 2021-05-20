#pragma once

#include "Components/Component.h"

namespace Components
{
	class Flag : public Component
	{
	private:
		bool m_flag;

	public:
		Flag(bool flag) : m_flag(flag)
		{
		}

		const bool& Get() const
		{
			return m_flag;
		}

		void Set(const bool flag)
		{
			m_flag = flag;
		}
	};
}