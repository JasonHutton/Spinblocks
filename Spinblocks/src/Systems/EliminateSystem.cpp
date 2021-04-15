#include "Systems/EliminateSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <set>

namespace Systems
{
	void EliminateSystem(entt::registry& registry, double currentFrameTime)
	{
		const auto& playAreaEnt = FindEntityByTag(registry, GetTagFromContainerType(containerType_t::PLAY_AREA));
		if (playAreaEnt == entt::null)
			throw std::runtime_error("Play Area entity is null!");
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaEnt);

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
			else
			{ // South
				if (rows.size() > 0)
				{
					unsigned int highestRow = *rows.rbegin();

					auto blockView = registry.view<Components::Block, Components::Moveable>();
					for (auto entity : blockView)
					{
						auto& moveable = blockView.get<Components::Moveable>(entity);

						if (moveable.GetCurrentCoordinate().Get().y < highestRow)
						{
							moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity, playAreaDirection.GetCurrentDownDirection(), static_cast<unsigned int>(rows.size()), true)));
							moveable.SetMovementState(Components::movementStates_t::HARD_DROP);
						}
					}
				}
			}
		}
		else
		{ // East/West

		}
	}
}