#pragma once

#include "Components/Component.h"

namespace Components
{
	class UIComponent : public Component
	{
	public:
		UIComponent(bool enabled = true) : Component(enabled)
		{

		}
	};
}