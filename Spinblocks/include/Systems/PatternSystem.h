#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void PatternSystem(entt::registry& registry, double currentFrameTime);
}