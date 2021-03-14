#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void MovementSystem(entt::registry& registry, double currentFrameTime);
}