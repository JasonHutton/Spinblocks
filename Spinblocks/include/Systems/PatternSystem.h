#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	int PatternSystem(entt::registry& registry, unsigned int lineWidth, double currentFrameTime);
}