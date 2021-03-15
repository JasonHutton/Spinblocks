#pragma once

#include "Component.h"

namespace Components
{
	class Flag : public Component
	{
	protected:
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