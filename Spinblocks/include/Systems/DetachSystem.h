#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void DetachSystem(entt::registry& registry, double currentFrameTime);
}