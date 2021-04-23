#include "Systems/DetachSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	bool IsBlockAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate)
	{
		auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Moveable>();
		for (auto entity : blockView)
		{
			auto& block = blockView.get<Components::Block>(entity);
			auto& blockCoordinate = blockView.get<Components::Coordinate>(entity);
			auto& blockMoveable = blockView.get<Components::Moveable>(entity);

			if (block.IsEnabled() && blockCoordinate.IsEnabled() && blockMoveable.IsEnabled())
			{
				if (blockCoordinate == coordinate)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool IsWallAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate, const moveDirection_t& moveDirection)
	{
		auto wallView = registry.view<Components::Wall, Components::Coordinate>();
		for (auto entity : wallView)
		{
			auto wall = wallView.get<Components::Wall>(entity);
			auto wallCoordinate = wallView.get<Components::Coordinate>(entity);

			if (wall.IsEnabled() && wallCoordinate.IsEnabled())
			{
				if (wallCoordinate == coordinate)
				{
					if (registry.all_of<Components::DirectionallyActive>(entity))
					{
						auto& dirWall = registry.get<Components::DirectionallyActive>(entity);
						if (dirWall.IsActive(moveDirection))
						{
							return true;
						}
						else
						{
							continue;
						}
					}
					else
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	// This is going to have problems with buffer area's that are up(and so wall isn't active there)
	bool IsCoordinateOutsidePlayArea(entt::registry& registry, const Components::Coordinate& coordinate)
	{
		if (coordinate.Get().x < BufferAreaDepth)
			return true;
		if (coordinate.Get().x >= BufferAreaDepth + PlayAreaWidth)
			return true;
		if (coordinate.Get().y < BufferAreaDepth)
			return true;
		if (coordinate.Get().y >= BufferAreaDepth + PlayAreaHeight)
			return true;

		return false;
	}

	int CountEmptyCellsBeneathBlock(entt::registry& registry, entt::entity cellEnt, const moveDirection_t& moveDirection, int count)
	{

		//if block
		//else if cell

		auto& originCellCoordinate = GetCoordinateOfEntity(registry, cellEnt);
		//auto originCellEnt = GetCellAtCoordinates2(registry, originCellCoordinate);
		//auto originCell = registry.get<Components::Cell>(originCellEnt);

		auto cellView = registry.view<Components::Cell, Components::Coordinate>();
		for (auto entity : cellView)
		{
			auto& cell = cellView.get<Components::Cell>(entity);
			auto& cellCoordinate = registry.get<Components::Coordinate>(entity);

			if (cell.IsEnabled() && cellCoordinate.IsEnabled())
			{
				if (cellCoordinate != originCellCoordinate)
					continue;

				/*if (cellCoordinate.GetParent() != originCellCoordinate.GetParent())
					continue;

				if (cellCoordinate.Get() != originCellCoordinate.Get())
					continue;
				*/

				// This cell is a wall. Stop.
				if (IsWallAtCoordinates(registry, cellCoordinate, moveDirection))
					return count;

				if (IsCoordinateOutsidePlayArea(registry, cellCoordinate))
					return count;

				if (!IsBlockAtCoordinates(registry, cellCoordinate))
				{
					count++;
				}

				// Next cell is null. Stop.
				if (cell.GetDirection(moveDirection) == entt::null)
					continue;

				return CountEmptyCellsBeneathBlock(registry, cell.GetDirection(moveDirection), moveDirection, count);
			}
		}

		return count;
	}

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

				int count = 0;
				count = CountEmptyCellsBeneathBlock(registry, entity, playAreaDirection.GetCurrentDownDirection(), count);



				//if (moveable.GetMovementState() == Components::movementStates_t::LOCKED)
				if(obstructable.GetIsObstructed())
				{
					obstructable.SetIsObstructed(false);
					moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity, playAreaDirection.GetCurrentDownDirection(), count, true))); // PlayAreaHeight is a const of 20. This is fine to be excessive with when the height is lower.
					moveable.SetMovementState(Components::movementStates_t::HARD_DROP);


					//CountEmptyCellsBeneathBlock();
				}
			}
		}
	}
}