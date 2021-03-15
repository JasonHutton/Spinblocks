#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void EliminateSystem(entt::registry& registry, double currentFrameTime);
}