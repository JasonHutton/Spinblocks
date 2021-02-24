#pragma once

#include <string>

namespace Components
{
	class Tag
	{
	protected:
		std::string m_tag;

	public:
		Tag(std::string tag) : m_tag(tag)
		{
		}

		const std::string Get() const
		{
			return m_tag;
		}
	};
}
