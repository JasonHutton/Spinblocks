#pragma once

#include "Components/Component.h"
#include "Components/Flag.h"

namespace Components
{
	class InheritScalingFromParent : public Flag
	{
	public:
		InheritScalingFromParent(bool flag) : Flag(flag)
		{
		}
	};
}