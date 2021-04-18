#include "Systems/EliminateSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <set>

#include "GameState.h"

namespace Systems
{
	void GenerationSystem(entt::registry& registry, double currentFrameTime)
	{
		const auto& bagAreaEnt = FindEntityByTag(registry, GetTagFromContainerType(containerType_t::BAG_AREA));
		if (bagAreaEnt == entt::null)
			throw std::runtime_error("Bag Area entity is null!");

		const auto& matrixEnt = FindEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX));
		if (matrixEnt == entt::null)
			throw std::runtime_error("Matrix entity is null!");
		
		auto& nodeOrder = registry.get<Components::NodeOrder>(bagAreaEnt);
		entt::entity nodeEnt = entt::null;

		if (currentFrameTime >= lastLockdownTime + generationTimeDelay && registry.view<Components::Controllable>().size() == 0)
		{
			entt::entity lastNode = nodeEnt;
			bool allSlotsFull = true;
			do
			{
				if (nodeEnt != entt::null)
					lastNode = nodeEnt;
				nodeEnt = nodeOrder.GetNode();
				if (nodeEnt != entt::null)
				{
					auto& node = registry.get<Components::QueueNode>(nodeEnt);
					if (node.GetContent() == entt::null)
						allSlotsFull = false;
				}
			} while (nodeEnt != entt::null);

			if (allSlotsFull)
			{
				auto& node = registry.get<Components::QueueNode>(lastNode);
				if (node.GetContent() != entt::null && node.GetDestination() == matrixEnt)
				{
					auto* tetromino = GetTetrominoFromEntity(registry, node.GetContent());
					if (tetromino != NULL)
					{
						entt::entity tet = node.GetContent();
						auto newCoordinate = Components::Coordinate(matrixEnt, GetTetrominoSpawnCoordinates(registry, GetTagFromContainerType(containerType_t::MATRIX), tetromino->GetType()));

						RelocateTetromino(registry, newCoordinate, tet);

						registry.emplace<Components::Controllable>(tet, newCoordinate.GetParent());
						if (registry.all_of<Components::Moveable>(tet))
						{
							auto& moveable = registry.get<Components::Moveable>(tet);
							moveable.SetMovementState(Components::movementStates_t::FALL);

							for (int i = 0; i < 4; i++)
							{
								auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
								if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
								{
									blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
								}
								else
								{
									blockMoveable.SetMovementState(Components::movementStates_t::FALL);
								}
							}
						}

						if (IsAnyBlockInTetrominoObstructed(registry, tet))
						{
							cout << "Obstructed on spawn! Game Over!" << endl;

							auto controllableView = registry.view<Components::Controllable>();
							for (auto controllable : controllableView)
							{
								registry.remove_if_exists<Components::Controllable>(controllable);
							}
							GameState::SetState(gameState_t::GAME_OVER);
						}
					}

					node.SetContent(entt::null);
				}
			}
			else
			{
				nodeEnt = entt::null;
				while ((nodeEnt = nodeOrder.GetNode()) != entt::null)
				{
					auto& node = registry.get<Components::QueueNode>(nodeEnt);
					auto& nodeCoordinates = registry.get<Components::Coordinate>(nodeEnt);
					if (node.GetContent() == entt::null)
					{
						if (registry.all_of<Components::Bag>(node.GetSource()))
						{
							auto& bag = registry.get<Components::Bag>(node.GetSource());
							node.SetContent(SpawnTetromino(registry, GetTagFromContainerType(containerType_t::BAG_AREA), nodeCoordinates, bag.PopTetromino(), false));
						}
						else if (registry.all_of<Components::QueueNode>(node.GetSource()))
						{
							auto& sourceNode = registry.get<Components::QueueNode>(node.GetSource());
							
							node.SetContent(sourceNode.GetContent());
							RelocateTetromino(registry, nodeCoordinates, node.GetContent());
							sourceNode.SetContent(entt::null);

							//auto& moveable = registry.get<Components::Moveable>(node.GetContent());
							//moveable.SetCurrentCoordinate(nodeCoordinates);
							//moveable.SetDesiredCoordinate(nodeCoordinates);
						}
					}
				}
			}
		}
	}
}