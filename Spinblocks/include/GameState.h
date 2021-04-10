#pragma once

#include "Globals.h"

namespace GameState
{
	void SetState(const gameState_t& state);
	const gameState_t& GetState();

	namespace
	{
		gameState_t gameState;
	}
};
