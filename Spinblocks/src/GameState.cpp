#include "GameState.h"

void GameState::SetState(const gameState_t& state)
{
	GameState::gameState = state;
}

const gameState_t& GameState::GetState()
{
	return GameState::gameState;
}
