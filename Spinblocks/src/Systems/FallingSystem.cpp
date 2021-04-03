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

			auto moveableView = registry.view<Components::Moveable, Components::Coordinate>(entt::exclude<Components::Follower>);
			for (auto entity : moveableView)
			{
				auto& moveable = moveableView.get<Components::Moveable>(entity);
				auto& coordinate = moveableView.get<Components::Coordinate>(entity);

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
							entt::entity desiredCell = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 1);
							if (GetCoordinateOfEntity(registry, desiredCell) != GetCoordinateOfEntity(registry, entity))
							{
								moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, desiredCell));
							}
							else
							{
								if (registry.has<Components::Obstructable>(entity))
								{
									auto& obstructable = registry.get<Components::Obstructable>(entity);
									//auto& block = GetBlockAtCoordinates(registry, tagOfContainerEntity, GetCoordinateOfEntity(registry, desiredCell));

									if (registry.has<Components::Obstructs>(entity))
									{
										obstructable.SetIsObstructed(true);
										obstructable.SetLastObstructedTime(currentFrameTime);
									}

									if (IsEntityTetromino(registry, entity))
									{
										auto* tetromino = GetTetrominoFromEntity(registry, entity);
										tetromino->SetAllBlocksObstructed(registry, true);
										tetromino->SetAllBlocksLastObstructedTime(registry, currentFrameTime);
									}
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