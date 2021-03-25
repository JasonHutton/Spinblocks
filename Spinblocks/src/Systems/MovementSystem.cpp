#include "Systems/MovementSystem.h"
#include "Utility.h"

namespace Systems
{
	void MovementSystem(entt::registry& registry, double currentFrameTime)
	{
		auto moveableView = registry.view<Components::Moveable, Components::Coordinate>();
		for (auto entity : moveableView)
		{
			auto& moveable = moveableView.get<Components::Moveable>(entity);
			auto& coordinate = moveableView.get<Components::Coordinate>(entity);

			if (moveable.IsEnabled() && coordinate.IsEnabled())
			{
				switch (moveable.GetMovementState())
				{
				case Components::movementStates_t::DEBUG_MOVE_UP:
				case Components::movementStates_t::SOFT_DROP:
				case Components::movementStates_t::FALL:
				{
					if (moveable.GetCurrentCoordinate() != moveable.GetDesiredCoordinate())
					{
						// Need to detect if a move is allowed before permitting it.
						coordinate = moveable.GetDesiredCoordinate();
						moveable.SetCurrentCoordinate(coordinate);
					}
					break;
				}
				case Components::movementStates_t::HARD_DROP:
				{
					if (moveable.GetCurrentCoordinate() != moveable.GetDesiredCoordinate())
					{
						// Need to detect if a move is allowed before permitting it.
						coordinate = moveable.GetDesiredCoordinate();
						moveable.SetCurrentCoordinate(coordinate);
						if (registry.has<Components::Obstructable>(entity))
						{
							auto& obstructable = registry.get<Components::Obstructable>(entity);
							if (registry.has<Components::Block>(entity))
							{
								obstructable.SetIsObstructed(true);
							}
						}
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
}