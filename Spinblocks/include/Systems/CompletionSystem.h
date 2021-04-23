#pragma once

#include <entt/entity/registry.hpp>

namespace Systems
{
	void CompletionSystem(entt::registry& registry, double currentFrameTime, int linesCleared);
}