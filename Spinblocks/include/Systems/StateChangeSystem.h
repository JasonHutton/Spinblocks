#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void StateChangeSystem(entt::registry& registry, double currentFrameTime);
}