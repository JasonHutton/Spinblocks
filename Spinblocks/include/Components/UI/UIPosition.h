#pragma once

#include "Components/UI/UIComponent.h"
#include "imgui.h"

namespace Components
{
	class UIPosition : public UIComponent
	{
	protected:
		ImVec2 m_position;
		ImVec2 m_pivot;

	public:
		UIPosition(const ImVec2& position = ImVec2(0.0f, 0.0f), const ImVec2& pivot = ImVec2(0.0f, 0.0f)) : m_position(position), m_pivot(pivot)
		{
		}

		const ImVec2& Get() const
		{
			return m_position;
		}

		void Set(const ImVec2& position)
		{
			m_position = position;
		}

		const ImVec2& GetPivot() const
		{
			return m_pivot;
		}

		void SetPivot(const ImVec2& pivot)
		{
			m_pivot = pivot;
		}
	};
}