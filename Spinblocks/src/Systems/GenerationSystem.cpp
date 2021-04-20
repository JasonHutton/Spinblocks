#include "Systems/EliminateSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <set>

#include "GameState.h"

namespace Systems
{
	void FillNodeQueue(entt::registry& registry, double currentFrameTime, entt::entity nodeEnt)
	{
		if (!registry.all_of<Components::QueueNode>(nodeEnt))
			return;

		auto& node = registry.get<Components::QueueNode>(nodeEnt);

		// Node is not empty.
		if (node.GetContent() != entt::null)
		{
			if (registry.all_of<Components::QueueNode>(node.GetDestination()))
			{
				auto& destination = registry.get<Components::QueueNode>(node.GetDestination());
				if (destination.GetContent() == entt::null)
				{ // Destination is empty
					auto& nodeCoordinates = registry.get<Components::Coordinate>(node.GetDestination());
					destination.SetContent(node.GetContent());
					node.SetContent(entt::null);
					RelocateTetromino(registry, nodeCoordinates, destination.GetContent());
				}
			}
		}
		else // Node is empty
		{
			if (registry.all_of<Components::QueueNode>(node.GetSource()))
			{ // Source is a node.
				// Look at that node.
				FillNodeQueue(registry, currentFrameTime, node.GetSource());
			}
			else if (registry.all_of<Components::Bag>(node.GetSource()))
			{ // Source is not a node. It's a bag component.
				// Spawn a new Tetromino here, I think.
				auto& bag = registry.get<Components::Bag>(node.GetSource());
				auto& nodeCoordinates = registry.get<Components::Coordinate>(nodeEnt);
				node.SetContent(SpawnTetromino(registry, GetTagFromContainerType(containerType_t::BAG_AREA), nodeCoordinates, bag.PopTetromino(), false));
			}
		}
	}

	void PopFromQueueIntoMatrix(entt::registry& registry, double currentFrameTime, entt::entity nodeEnt)
	{
		if (!registry.all_of<Components::QueueNode>(nodeEnt))
			return;

		auto& node = registry.get<Components::QueueNode>(nodeEnt);

		entt::entity tet = node.GetContent();
		node.SetContent(entt::null);

		auto* tetromino = GetTetrominoFromEntity(registry, tet);
		if (tetromino != NULL)
		{
			auto newCoordinate = Components::Coordinate(node.GetDestination(), GetTetrominoSpawnCoordinates(registry, GetTagFromContainerType(containerType_t::MATRIX), tetromino->GetType()));

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
	}

	int CountTetrominos(entt::registry& registry)
	{
		int tetCount = 0;

		auto itets = registry.view<Components::ITetromino>();
		for (auto entity : itets)
		{
			tetCount++;
		}
		auto jtets = registry.view<Components::JTetromino>();
		for (auto entity : jtets)
		{
			tetCount++;
		}
		auto ltets = registry.view<Components::LTetromino>();
		for (auto entity : ltets)
		{
			tetCount++;
		}
		auto otets = registry.view<Components::OTetromino>();
		for (auto entity : otets)
		{
			tetCount++;
		}
		auto stets = registry.view<Components::STetromino>();
		for (auto entity : stets)
		{
			tetCount++;
		}
		auto ztets = registry.view<Components::ZTetromino>();
		for (auto entity : ztets)
		{
			tetCount++;
		}
		auto ttets = registry.view<Components::TTetromino>();
		for (auto entity : ttets)
		{
			tetCount++;
		}

		return tetCount;
	}

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

		int tetCount = CountTetrominos(registry);
		if (tetCount != 5 && tetCount != 4) // 5 is the default state when one is falling, 4 is the default state when one locks down, before a new one is generated
		{
			int q = 0;
			q++;
		}
		cout << "Tetrominos in existence: " << tetCount << endl;

		// Fill the queue if it's not full
		bool atLeastOneNodeIsEmpty = false;
		do
		{
			atLeastOneNodeIsEmpty = false;
			auto nodeView = registry.view<Components::QueueNode>();
			for (auto entity : nodeView)
			{
				auto& node = registry.get<Components::QueueNode>(entity);
				if (node.GetContent() == entt::null)
				{
					atLeastOneNodeIsEmpty = true;
				}
			}

			if (atLeastOneNodeIsEmpty)
			{
				nodeEnt = nodeOrder.GetNode();
				if (nodeEnt != entt::null)
				{
					auto& node = registry.get<Components::QueueNode>(nodeEnt);
					// First node is the source node. Bag area entity 3

					// This node is empty.
					if (node.GetContent() == entt::null)
					{
						FillNodeQueue(registry, currentFrameTime, nodeEnt);
					}
				}
			}
		} while (atLeastOneNodeIsEmpty);

		if (currentFrameTime >= lastLockdownTime + generationTimeDelay && registry.view<Components::Controllable>().size() == 0)
		{
			auto nodeView = registry.view<Components::QueueNode>();
			for (auto entity : nodeView)
			{
				auto& node = registry.get<Components::QueueNode>(entity);

				if (node.GetDestination() != FindEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX)))
					continue;

				if (node.GetContent() == entt::null)
					continue;
				
				PopFromQueueIntoMatrix(registry, currentFrameTime, entity);
			}
		}
	}
}