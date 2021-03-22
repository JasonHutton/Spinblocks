#include "Systems/EliminateSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <set>

namespace Systems
{
	void EliminateSystem(entt::registry& registry, double currentFrameTime)
	{
		auto rows = std::set<unsigned int>();

		// Clear all hitlist marked ents
		auto hittableView = registry.view<Components::Block, Components::Hittable>();
		for (auto entity : hittableView)
		{
			rows.insert(GetCoordinateOfEntity(registry, entity).Get().y); // Note all unique rows that have been cleared.

			registry.destroy(entity);
		}


		if (rows.size() > 0)
		{
			unsigned int lowestRow = *rows.begin();

			auto blockView = registry.view<Components::Block, Components::Moveable>();
			for (auto entity : blockView)
			{
				auto& moveable = blockView.get<Components::Moveable>(entity);

				if (moveable.GetCurrentCoordinate().Get().y > lowestRow)
				{
					moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, static_cast<unsigned int>(rows.size()), true)));
					moveable.SetMovementState(Components::movementStates_t::HARD_DROP);
				}
			}
		}
	}
}