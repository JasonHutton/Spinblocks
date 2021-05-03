#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	bool MovementSystem(entt::registry& registry, double currentFrameTime);
}