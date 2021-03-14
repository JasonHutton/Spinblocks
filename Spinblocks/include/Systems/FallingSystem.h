#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void FallingSystem(entt::registry& registry, double currentFrameTime);
}