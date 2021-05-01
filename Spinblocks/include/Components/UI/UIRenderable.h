#pragma once

#include "Components/UI/UIComponent.h"

namespace Components
{
	class UIRenderable : public UIComponent
	{
	public:
		UIRenderable(bool enabled = true) : UIComponent(enabled)
		{
		}
	};
}