#pragma once

#include "Components/UI/UIWidget.h"
#include "imgui.h"
#include <string>
#include <vector>

namespace Components
{
	class UIMenuPanel : public UIWidget
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

		std::vector<entt::entity> m_widgets;

	public:
		UIMenuPanel(const std::string windowName, bool enabled = true) : UIWidget(windowName, enabled)
		{
		
		}

		void AddWidget(entt::entity entity)
		{
			m_widgets.push_back(entity);
		}

		// Just taking some shortcuts with access on this. FIXME TODO
		std::vector<entt::entity>& AccessWidgets()
		{
			return m_widgets;
		}

		void DisplayElement() override
		{
		}

		/*
		void DisplayElement(entt::registry& registry) override
		{
			
			for (entt::entity entity : m_widgets)
			{
				Components::UIWidget* widget = GetUIWidgetFromEntity(registry, entity);
				if (registry.all_of<Components::UIMenuButton>(entity))
				{
					auto btn = registry.get<Components::UIMenuButton>(entity);
				}
			}
		}*/
	};
}