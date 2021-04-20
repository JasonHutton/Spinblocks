#include "Systems/BoardRotateSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	rotationDirection_t BoardRotateSystem(entt::registry& registry, double currentFrameTime, rotationDirection_t rotationDirection)
	{
		return rotationDirection_t::NONE;
	}
}