#include "Systems/StateChangeSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	void StateChangeSystem(entt::registry& registry, double currentFrameTime)
	{
		auto blockView = registry.view<Components::Block>();
		for (auto entity : blockView)
		{
			auto& block = blockView.get<Components::Block>(entity);
			auto& moveable = registry.get<Components::Moveable>(entity);
			auto& coordinate = registry.get<Components::Coordinate>(entity);

			if (block.IsEnabled() && moveable.IsEnabled())
			{
				switch (moveable.GetMovementState())
				{
				case Components::movementStates_t::FALL:
					if (block.GetIsFallingObstructed() && currentFrameTime >= moveable.GetLastObstructedTime() + lockdownDelay)
					{
						moveable.SetMovementState(Components::movementStates_t::LOCKED);
						registry.remove_if_exists<Components::Controllable>(entity);
					}
					// Do nothing
					//lastFallUpdate = currentFrameTime; // Reset the fall time, to avoid a change of state here resulting in an immediate fall, which manifests as a double-move, which feels bad.
					//moveable.SetMovementState(Components::movementStates_t::FALL);
					break;
				case Components::movementStates_t::DEBUG_MOVE_UP:
					lastFallUpdate = currentFrameTime; // Reset the fall time, to avoid a change of state here resulting in an immediate fall, which manifests as a double-move, which feels bad.
					block.SetIsFallingObstructed(false);
					moveable.SetMovementState(Components::movementStates_t::FALL); // Reset to falling state for the next tick.
					break;
				case Components::movementStates_t::SOFT_DROP:
					if (block.GetIsFallingObstructed())
					{
						// This never gets called, probably due to the fall state being set places instead, and the obstructed flag being set from the fall state. Not necessarily a problem.
						moveable.SetMovementState(Components::movementStates_t::LOCKED);
						registry.remove_if_exists<Components::Controllable>(entity);
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
					break;
				default:
					break;
				}
			}
		}
	}
}