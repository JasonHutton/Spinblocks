#include "Systems/StateChangeSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	void StateChangeSystem(entt::registry& registry, double currentFrameTime)
	{
		/*auto tetrominoView = registry.view<Components::Moveable, Components::Coordinate>(entt::exclude<Components::Obstructable>);
		for (auto entity : tetrominoView)
		{
			if (!IsEntityTetromino(registry, entity)) // There should be a better way to do this. Look into it later. FIXME TODO
				continue;

			auto& moveable = blockView.get<Components::Moveable>(entity);
			auto& coordinate = blockView.get<Components::Coordinate>(entity);


		}*/

		auto blockView = registry.view<Components::Obstructable, Components::Moveable, Components::Coordinate>();
		for (auto entity : blockView)
		{
			//auto& block = registry.get<Components::Block>(entity);
			auto& moveable = blockView.get<Components::Moveable>(entity);
			auto& coordinate = blockView.get<Components::Coordinate>(entity);
			auto& obstructable = blockView.get<Components::Obstructable>(entity);

			if (moveable.IsEnabled() && coordinate.IsEnabled() && obstructable.IsEnabled())
			{
				switch (moveable.GetMovementState())
				{
				case Components::movementStates_t::FALL:
					if (obstructable.GetIsObstructed() && currentFrameTime >= obstructable.GetLastObstructedTime() + lockdownDelay)
					{
						moveable.SetMovementState(Components::movementStates_t::LOCKED);
						registry.remove_if_exists<Components::Controllable>(entity);
						lastLockdownTime = currentFrameTime;
					}

					if (IsEntityTetromino(registry, entity))
					{
						auto* tetromino = GetTetrominoFromEntity(registry, entity);

						if (tetromino->GetAreAllBlocksObstructed(registry) && currentFrameTime >= tetromino->GetAllBlocksLastObstructedTime(registry) + lockdownDelay)
						{
							tetromino->SetAllBlocksMovementState(registry, Components::movementStates_t::LOCKED);
							lastLockdownTime = currentFrameTime;
						}
					}
					
					// Do nothing
					//lastFallUpdate = currentFrameTime; // Reset the fall time, to avoid a change of state here resulting in an immediate fall, which manifests as a double-move, which feels bad.
					//moveable.SetMovementState(Components::movementStates_t::FALL);
					break;
				case Components::movementStates_t::DEBUG_MOVE_UP:
					lastFallUpdate = currentFrameTime; // Reset the fall time, to avoid a change of state here resulting in an immediate fall, which manifests as a double-move, which feels bad.

					obstructable.SetIsObstructed(false);
					moveable.SetMovementState(Components::movementStates_t::FALL); // Reset to falling state for the next tick.
					if (IsEntityTetromino(registry, entity))
					{
						auto* tetromino = GetTetrominoFromEntity(registry, entity);
						tetromino->SetAllBlocksObstructed(registry, false);
						//tetromino->SetAllBlocksMovementState(registry, Components::movementStates_t::FALL);
					}
					
					
					break;
				case Components::movementStates_t::SOFT_DROP:
					if (obstructable.GetIsObstructed())
					{
						// This never gets called, probably due to the fall state being set places instead, and the obstructed flag being set from the fall state. Not necessarily a problem.
						moveable.SetMovementState(Components::movementStates_t::LOCKED);
						registry.remove_if_exists<Components::Controllable>(entity);
						lastLockdownTime = currentFrameTime;

						if (IsEntityTetromino(registry, entity))
						{
							auto* tetromino = GetTetrominoFromEntity(registry, entity);

							if (tetromino->GetAreAllBlocksObstructed(registry))
							{
								tetromino->SetAllBlocksMovementState(registry, Components::movementStates_t::LOCKED);
								lastLockdownTime = currentFrameTime;
							}
						}
					}
					else
					{
						lastFallUpdate = currentFrameTime; // Reset the fall time, to avoid a change of state here resulting in an immediate fall, which manifests as a double-move, which feels bad.
						//block.SetIsFallingObstructed(false);
						moveable.SetMovementState(Components::movementStates_t::FALL); // Reset to falling state for the next tick.
					}
					break;
				case Components::movementStates_t::HARD_DROP:
					lastFallUpdate = currentFrameTime; // Reset the fall time, to avoid a change of state here resulting in an immediate fall, which manifests as a double-move, which feels bad.
					moveable.SetMovementState(Components::movementStates_t::LOCKED);
					registry.remove_if_exists<Components::Controllable>(entity);
					lastLockdownTime = currentFrameTime;

					if (IsEntityTetromino(registry, entity))
					{
						auto* tetromino = GetTetrominoFromEntity(registry, entity);

						if (tetromino->GetAreAllBlocksObstructed(registry))
						{
							tetromino->SetAllBlocksMovementState(registry, Components::movementStates_t::LOCKED);
							lastLockdownTime = currentFrameTime;
						}
					}
					break;
				default:
					break;
				}
			}
		}

		// Kill tetrominoes, leaving their blocks behind, after they're locked down.
		auto tetrominoView = registry.view<Components::Moveable, Components::Coordinate>(entt::exclude<Components::Obstructs>);
		for (auto entity : tetrominoView)
		{
			if (!IsEntityTetromino(registry, entity)) // There should be a better way to do this. Look into it later. FIXME TODO
				continue;

			auto* tetromino = GetTetrominoFromEntity(registry, entity);
			if (tetromino->GetAreAllBlocksLocked(registry))
			{
				for (int i = 0; i < 4; i++)
				{
					registry.remove_if_exists<Components::Follower>(tetromino->GetBlock(i));
				}

				registry.destroy(entity);
			}
		}
	}
}