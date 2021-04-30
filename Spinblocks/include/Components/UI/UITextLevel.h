#pragma once

#include "Components/UI/UIText.h"
#include "Systems/SystemShared.h"

namespace Components
{
	class UITextLevel : public UIText
	{
	public:
		UITextLevel()
		{
		}

		void DisplayElement() override
		{
			ImGui::Text("Level: %d", GameLevel);
		}
	};
}