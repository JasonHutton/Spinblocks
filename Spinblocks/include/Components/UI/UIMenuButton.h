#pragma once

#include "Components/UI/UIWidget.h"
#include "imgui.h"
#include <iostream>

namespace Components
{
	class UIMenuButton : public UIWidget
	{
	protected:
		(void)(*m_activate);
		GLFWwindow* m_window;
	
		// Dump doing GUI as ECS due to having to pass around fucntion pointers or setup a message pump just for it.
	public:
		UIMenuButton(const std::string buttonName, void* activate, GLFWwindow* window, bool enabled = true) : m_activate(activate), m_window(window), UIWidget(buttonName, enabled)
		{
		}

		void DisplayElement() override
		{
			if (ImGui::Button(m_windowName.c_str()))
			{
				(m_activate)();// (m_window, true);
			}
		}
	};
}