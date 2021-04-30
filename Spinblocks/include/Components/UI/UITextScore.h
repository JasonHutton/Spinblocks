#pragma once

#include "Components/UI/UIText.h"
#include "Systems/SystemShared.h"

namespace Components
{
	class UITextScore : public UIText
	{
	public:
		UITextScore()
		{
		}

		void DisplayElement() override
		{
			ImGui::Text("Score: %d", GameScore);
		}
	};
}