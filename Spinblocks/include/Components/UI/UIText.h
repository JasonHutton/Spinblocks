#pragma once

#include "Components/UI/UIComponent.h"
#include <string>

namespace Components
{
	class UIText : public UIComponent
	{
	protected:
		std::string m_text;

	public:
		UIText(const std::string& text = "") : m_text(text)
		{
		}

		virtual void DisplayElement()
		{
			ImGui::Text(m_text.c_str());
		}
	};
}