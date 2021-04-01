#pragma once

#include "Components/Component.h"

namespace Components
{
	class GameObject : public Component
	{
	public:
		GameObject()
		{
			Enable();
		}
	};
}