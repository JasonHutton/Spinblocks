#pragma once

#include <entt/entity/registry.hpp>

#include <vector>
#include "Components/Block.h"

#include "Globals.h"

namespace Systems
{
	statesChanged_t StateChangeSystem(entt::registry& registry, double currentFrameTime, std::vector<BlockLockData>& blockLockData);
}