#pragma once

#include <entt/entity/registry.hpp>

enum class rotationDirection_t;

namespace Systems
{
	rotationDirection_t BoardRotateSystem(entt::registry& registry, double currentFrameTime, rotationDirection_t rotationDirection);
}