#pragma once

#include <entt/entity/registry.hpp>

#include <vector>
#include "Components/Block.h"

namespace Systems
{
	void StateChangeSystem(entt::registry& registry, double currentFrameTime, std::vector<BlockLockData>& blockLockData);
}