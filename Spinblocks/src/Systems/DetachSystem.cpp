#include "Systems/DetachSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	void DetachSystem(entt::registry& registry, double currentFrameTime)
	{
		auto blockView = registry.view<Components::Block, Components::Moveable, Components::Coordinate, Components::Obstructable>();
		for (auto entity : blockView)
		{
			auto& block = blockView.get<Components::Block>(entity);
			auto& moveable = blockView.get<Components::Moveable>(entity);
			auto& coordinate = blockView.get<Components::Coordinate>(entity);
			auto& obstructable = blockView.get<Components::Obstructable>(entity);

			if (block.IsEnabled() && moveable.IsEnabled() && coordinate.IsEnabled())
			{
				if (coordinate.GetParent() != FindEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX))) // Don't pattern anything not in the play area matrix
					continue;

				auto& playAreaRefEnt = registry.get<Components::ReferenceEntity>(moveable.GetCurrentCoordinate().GetParent());
				auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaRefEnt.Get());

				//if (moveable.GetMovementState() == Components::movementStates_t::LOCKED)
				if(obstructable.GetIsObstructed())
				{
					obstructable.SetIsObstructed(false);
					moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity, playAreaDirection.GetCurrentDownDirection(), PlayAreaHeight + (BufferAreaDepth * 2)))); // PlayAreaHeight is a const of 20. This is fine to be excessive with when the height is lower.
					moveable.SetMovementState(Components::movementStates_t::HARD_DROP);

					// THIS IS WRONG
				}
			}
		}
	}
}