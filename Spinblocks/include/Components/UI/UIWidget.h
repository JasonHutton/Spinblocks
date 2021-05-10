#pragma once

#include "Components/UI/UIComponent.h"
#include "imgui.h"
#include <string>

namespace Components
{
	class UIWidget : public UIComponent
	{
	protected:
		ImGuiWindowFlags m_windowFlags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoMove;
		ImGuiCond m_condition = ImGuiCond_Always;
		std::string m_windowName;

	public:
		UIWidget(const std::string windowName, bool enabled = true) : m_windowName(windowName), UIComponent(enabled)
		{
		
		}

		const std::string& GetWindowName() const
		{
			return m_windowName;
		}

		const ImGuiCond& GetCondition() const
		{
			return m_condition;
		}

		void SetWindowFlags(ImGuiWindowFlags flags)
		{
			m_windowFlags = flags;
		}

		const ImGuiWindowFlags& GetWindowFlags() const
		{
			return m_windowFlags;
		}

		virtual void DisplayElement() = 0;
	};
}