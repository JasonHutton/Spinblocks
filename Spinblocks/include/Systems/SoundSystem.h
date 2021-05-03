#pragma once

#include <entt/entity/registry.hpp>

#include "Globals.h"

namespace Systems
{
	void SoundSystem(entt::registry& registry, const bool& aPieceMoved, const statesChanged_t& statesChanged, const int& linesMatched);
}