#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void GenerationSystem(entt::registry& registry, double currentFrameTime);
}