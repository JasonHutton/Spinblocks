#include "Systems/FallingSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	void FallingSystem(entt::registry& registry, double currentFrameTime)
	{
		if (currentFrameTime >= lastFallUpdate + FallSpeed)
		{
			lastFallUpdate = currentFrameTime;

			auto moveableViewx = registry.view<Components::Moveable, Components::Coordinate>();
			for (auto entity : moveableViewx)
			{
				auto& moveable = moveableViewx.get<Components::Moveable>(entity);
				auto& coordinate = moveableViewx.get<Components::Coordinate>(entity);

				if (moveable.IsEnabled() && coordinate.IsEnabled())
				{
					switch (moveable.GetMovementState())
					{
					case Components::movementStates_t::FALL:
					{
						Components::Coordinate movableCoord = moveable.GetCurrentCoordinate();
						entt::entity moveableParentEntity = movableCoord.GetParent();
						std::string tagOfContainerEntity = FindTagOfContainerEntity(registry, moveableParentEntity);

						auto& cell = GetCellAtCoordinates(registry, tagOfContainerEntity, movableCoord); // If can't find, don't move
						if (cell.IsEnabled())
						{
							if (CanOccupyCell(registry, FindTagOfContainerEntity(registry, cell.GetParent()), cell.GetSouth()))
							{
								moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, cell.GetSouth()));
							}
							else
							{
								if (registry.has<Components::Block>(entity))
								{
									auto& block = registry.get<Components::Block>(entity);
									block.SetIsFallingObstructed(true);
									moveable.SetLastObstructedTime(currentFrameTime);
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
}