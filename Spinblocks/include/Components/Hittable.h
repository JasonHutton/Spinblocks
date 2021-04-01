#pragma once

#include "Components/Component.h"
#include "Components/Flag.h"

namespace Components
{
	class Hittable : public Flag
	{
	protected:

	public:
		Hittable(bool flag = true) : Flag(flag)
		{
		}
	};
}