#include "Systems/EliminateSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <set>

namespace Systems
{
	void GenerationSystem(entt::registry& registry, double currentFrameTime)
	{
		const auto& bagAreaEnt = FindEntityByTag(registry, GetTagFromContainerType(containerType_t::BAG_AREA));
		if (bagAreaEnt == entt::null)
			throw std::runtime_error("Bag Area entity is null!");
		
		auto& nodeOrder = registry.get<Components::NodeOrder>(bagAreaEnt);
		
		entt::entity nodeEnt = entt::null;
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
					sourceNode.SetContent(entt::null);
					
					auto& moveable = registry.get<Components::Moveable>(node.GetContent());
					moveable.SetCurrentCoordinate(nodeCoordinates);
					moveable.SetDesiredCoordinate(nodeCoordinates);
				}
			}
		}

		/*
		if (playAreaDirection.GetCurrentOrientation() == moveDirection_t::NORTH || playAreaDirection.GetCurrentOrientation() == moveDirection_t::SOUTH)
		{
			auto rows = std::set<unsigned int>();

			// Clear all hitlist marked ents
			auto hittableView = registry.view<Components::Block, Components::Hittable>();
			for (auto entity : hittableView)
			{
				rows.insert(GetCoordinateOfEntity(registry, entity).Get().y); // Note all unique rows that have been cleared.

				registry.destroy(entity);
			}

			if (playAreaDirection.GetCurrentOrientation() == moveDirection_t::NORTH)
			{ // North
				if (rows.size() > 0)
				{
					unsigned int lowestRow = *rows.begin();

					auto blockView = registry.view<Components::Block, Components::Moveable>();
					for (auto entity : blockView)
					{
						auto& moveable = blockView.get<Components::Moveable>(entity);

						if (moveable.GetCurrentCoordinate().Get().y > lowestRow)
						{
							moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity, playAreaDirection.GetCurrentDownDirection(), static_cast<unsigned int>(rows.size()), true)));
							moveable.SetMovementState(Components::movementStates_t::HARD_DROP);
						}
					}
				}
			}
		}*/
	}
}