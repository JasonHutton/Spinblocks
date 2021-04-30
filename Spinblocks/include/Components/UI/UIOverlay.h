#pragma once

#include "Components/UI/UIComponent.h"
#include "imgui.h"

namespace Components
{
	class UIOverlay : public UIComponent
	{
	protected:
		ImGuiWindowFlags m_windowFlags = ImGuiWindowFlags_NoDecoration |
										ImGuiWindowFlags_AlwaysAutoResize |
										ImGuiWindowFlags_NoSavedSettings |
										ImGuiWindowFlags_NoFocusOnAppearing |
										ImGuiWindowFlags_NoNav |
										ImGuiWindowFlags_NoMove |
										ImGuiWindowFlags_NoInputs;
		ImGuiCond m_condition = ImGuiCond_Always;

	public:
		UIOverlay()
		{
		
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
	};
}