#pragma once

#include "Component.h"

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