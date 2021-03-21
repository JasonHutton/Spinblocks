#include "Utility.h"

// Ensure containerType_t and this function are in sync
const std::string GetTagFromContainerType(const containerType_t& t)
{
	switch (t)
	{
	case containerType_t::PLAY_AREA:
		return "Play Area";
	case containerType_t::MATRIX:
		return "Matrix";
	case containerType_t::BUFFER:
		return "Buffer";
	case containerType_t::BAG_AREA:
		return "Bag Area";
	default:
		throw std::runtime_error("Unable to convert container type to tag!");
	}
}

entt::entity FindContainerEntityByTag(entt::registry& registry, const std::string& tagName)
{
	entt::entity foundEntity = entt::null;

	auto containerView = registry.view<Components::Container2, Components::Tag>();
	for (auto entity : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(entity);
		auto& tag = containerView.get<Components::Tag>(entity);
		if (container2.IsEnabled() && tag.IsEnabled() && tag.Get() == tagName)
		{
			foundEntity = entity;
		}
	}

	return foundEntity;
}

const std::string FindTagOfContainerEntity(entt::registry& registry, const entt::entity& containerEntity)
{
	auto containerView = registry.view<Components::Container2, Components::Tag>();
	for (auto entity : containerView)
	{
		if (containerEntity != entity)
			continue;

		auto& container2 = containerView.get<Components::Container2>(entity);
		auto& tag = containerView.get<Components::Tag>(entity);
		if (container2.IsEnabled() && tag.IsEnabled())
		{
			return tag.Get();
		}
	}

	throw std::runtime_error("Unable to find tag of container entity!");
}

bool CanOccupyCell(entt::registry& registry, const entt::entity& cellEntity, const bool& disableObstruction)
{
	if (cellEntity == entt::null)
		return false;

	if (!registry.has<Components::Cell>(cellEntity))
		return false;

	if (!registry.has<Components::Coordinate>(cellEntity))
		return false;

	auto& cell = registry.get<Components::Cell>(cellEntity);
	auto& cellCoordinate = registry.get<Components::Coordinate>(cellEntity);

	if (!cell.IsEnabled() || !cellCoordinate.IsEnabled())
		return false;

	// Don't check for obstructions. Sooooo, we're able to move into here regardless without further checks.
	if (disableObstruction)
		return true;

	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto blockEntity : blockView)
	{
		auto& block = registry.get<Components::Block>(blockEntity);
		auto& blockCoordinate = registry.get<Components::Coordinate>(blockEntity);

		if (block.IsEnabled() && blockCoordinate.IsEnabled())
		{
			auto& container2 = registry.get<Components::Container2>(block.Get());

			if (blockCoordinate == cellCoordinate)
			{
				// We're short circuiting here on the first match. We probably want to check for container first. (eg: When we've got multiple reference containers in use.)
				return false;
			}
		}
	}

	return true;
}

const Components::Coordinate& GetCoordinateOfEntity(entt::registry& registry, const entt::entity& entity)
{
	if (entity == entt::null)
		throw std::runtime_error("Entity is NULL!");

	if (!registry.has<Components::Coordinate>(entity))
		throw std::runtime_error("Entity does not have component!");

	return registry.get<Components::Coordinate>(entity);
}

const Components::Cell& GetCellOfEntity(entt::registry& registry, const entt::entity& entity)
{
	if (entity == entt::null)
		throw std::runtime_error("Entity is NULL!");

	if (!registry.has<Components::Cell>(entity))
		throw std::runtime_error("Entity does not have component!");

	return registry.get<Components::Cell>(entity);
}

Components::Cell& GetCellAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate)
{
	auto cellView = registry.view<Components::Cell, Components::Coordinate>();
	for (auto entity : cellView)
	{
		auto& cell = cellView.get<Components::Cell>(entity);

		auto& cellCoordinate = registry.get<Components::Coordinate>(entity);

		if (cell.IsEnabled() && cellCoordinate.IsEnabled())
		{
			auto& container2 = registry.get<Components::Container2>(cell.GetParent());
			auto& tag = registry.get<Components::Tag>(cell.GetParent()); // We'll be wanting to check which container we're working with later. (eg: Play Area, Hold, Preview, (which play area?))
			if (!tag.IsEnabled() || containerTag != tag.Get())
				continue;

			if (cellCoordinate == coordinate)
			{
				// We're short circuiting here on the first match. We probably want to check for container first. (eg: When we've got multiple reference containers in use.)
				return cell;
			}
		}
	}

	throw std::runtime_error("Unable to find Cell at coordinates!");
}

entt::entity GetCellAtCoordinates2(entt::registry& registry, const Components::Coordinate& coordinate)
{
	auto cellView = registry.view<Components::Cell, Components::Coordinate>();
	for (auto entity : cellView)
	{
		auto& cell = cellView.get<Components::Cell>(entity);
		auto& cellCoordinate = cellView.get<Components::Coordinate>(entity);

		if (cell.IsEnabled() && cellCoordinate.IsEnabled())
		{
			if (!registry.has<Components::Container2>(cell.GetParent()))
				continue;
			if (!registry.has<Components::Tag>(cell.GetParent()))
				continue;

			auto& container2 = registry.get<Components::Container2>(cell.GetParent());

			// The cell we've found shares the same coordinates as what we've specified.
			if (cellCoordinate == coordinate)
			{
				return entity;
			}
		}
	}

	return entt::null;
}

entt::entity GetCellLinkAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate, const moveDirection_t& direction)
{
	auto cellView = registry.view<Components::CellLink, Components::Coordinate>();
	for (auto entity : cellView)
	{
		auto& cellCoordinate = cellView.get<Components::Coordinate>(entity);
		auto& cellLink = cellView.get<Components::CellLink>(entity);

		if (cellLink.GetDirection() != direction)
			continue;

		if (cellCoordinate.IsEnabled() && cellLink.IsEnabled())
		{
			if (registry.has<Components::Cell>(cellLink.GetSource()))
			{
				auto& cell = registry.get<Components::Cell>(cellLink.GetSource());
				if (cell.IsEnabled())
				{
					if (cellCoordinate == coordinate)
					{
						return entity;
					}
				}
			}
		}
	}

	return entt::null;
}

const Components::Block& GetBlockAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate)
{
	auto blockView = registry.view<Components::Block>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);

		if (block.IsEnabled())
		{
			auto& container2 = registry.get<Components::Container2>(block.Get());
			auto& tag = registry.get<Components::Tag>(block.Get()); // We'll be wanting to check which container we're working with later. (eg: Play Area, Hold, Preview, (which play area?))
			if (!tag.IsEnabled() || containerTag != tag.Get())
				continue;

			auto& blockCoordinate = registry.get<Components::Coordinate>(block.Get());
			if (blockCoordinate == coordinate)
			{
				// We're short circuiting here on the first match. We probably want to check for container first. (eg: When we've got multiple reference containers in use.)
				return block;
			}
		}
	}

	throw std::runtime_error("Unable to find Block at coordinates!");
}

/*
* A Block will always have the following components in the same entity: (Probably want some way to enforce that somewhere, but whatever for now.)
* There can be other components, these are just relevant to this function.
* Coordinate
* Moveable (Even lockeed blocks are still moveable.)
* Block
* Controllable (NOT ALWAYS THERE. Only there on the currently active piece.)
*/
entt::entity MoveBlockInDirection(entt::registry& registry, const entt::entity& blockEnt, const moveDirection_t& direction, const unsigned int& distance, const bool& disableObstruction)
{
	auto& coordinate = registry.get<Components::Coordinate>(blockEnt);
	auto& moveable = registry.get<Components::Moveable>(blockEnt);

	entt::entity cellEnt = GetCellAtCoordinates2(registry, coordinate);
	if (cellEnt == entt::null)
		return entt::null;

	entt::entity newCellEnt = cellEnt;

	for (unsigned int i = 0; i < distance; i++)
	{
		// We're moving a bunch in this function. Update our current coordiantes while doing so.
		if (i > 0 && registry.has<Components::Coordinate>(newCellEnt))
		{
			auto& newCoordinate = registry.get<Components::Coordinate>(newCellEnt);
			coordinate.Set(newCoordinate.Get());
			coordinate.SetParent(newCoordinate.GetParent());
		}

		entt::entity tempCellEnt = newCellEnt;
		if (!registry.has<Components::Cell>(tempCellEnt))
			continue;
		Components::Cell& tempCell = registry.get<Components::Cell>(tempCellEnt);

		switch (direction)
		{
		case moveDirection_t::NORTH:
			if (CanOccupyCell(registry, tempCell.GetNorth(), disableObstruction))
			{
				newCellEnt = tempCell.GetNorth();
			}
			else
			{
				if (tempCell.GetNorth() == entt::null)
				{
					entt::entity cellLinkEnt = GetCellLinkAtCoordinates(registry, coordinate, direction);
					if (cellLinkEnt != entt::null)
					{
						auto& cellLink = registry.get<Components::CellLink>(cellLinkEnt);

						// Only move into the linked cell destination, if we can occupy it.
						if (CanOccupyCell(registry, cellLink.GetDestination(), disableObstruction))
						{
							newCellEnt = cellLink.GetDestination();

							if (registry.has<Components::Controllable>(blockEnt))
							{
								auto& controllable = registry.get<Components::Controllable>(blockEnt);
								auto& cellLinkDestination = registry.get<Components::Cell>(cellLink.GetDestination());
								controllable.Set(cellLinkDestination.GetParent());
							}
						}
					}
				}
			}
			break;
		case moveDirection_t::SOUTH:
			if (CanOccupyCell(registry, tempCell.GetSouth(), disableObstruction))
			{
				newCellEnt = tempCell.GetSouth();
			}
			else
			{
				if (tempCell.GetSouth() == entt::null)
				{
					entt::entity cellLinkEnt = GetCellLinkAtCoordinates(registry, coordinate, direction);
					if (cellLinkEnt != entt::null)
					{
						auto& cellLink = registry.get<Components::CellLink>(cellLinkEnt);

						// Only move into the linked cell destination, if we can occupy it.
						if (CanOccupyCell(registry, cellLink.GetDestination(), disableObstruction))
						{
							newCellEnt = cellLink.GetDestination();

							if (registry.has<Components::Controllable>(blockEnt))
							{
								auto& controllable = registry.get<Components::Controllable>(blockEnt);
								auto& cellLinkDestination = registry.get<Components::Cell>(cellLink.GetDestination());
								controllable.Set(cellLinkDestination.GetParent());
							}
						}
					}
				}
			}
			break;
		case moveDirection_t::EAST:
			if (CanOccupyCell(registry, tempCell.GetEast(), disableObstruction))
			{
				newCellEnt = tempCell.GetEast();
			}
			else
			{
				if (tempCell.GetEast() == entt::null)
				{
					entt::entity cellLinkEnt = GetCellLinkAtCoordinates(registry, coordinate, direction);
					if (cellLinkEnt != entt::null)
					{
						auto& cellLink = registry.get<Components::CellLink>(cellLinkEnt);

						// Only move into the linked cell destination, if we can occupy it.
						if (CanOccupyCell(registry, cellLink.GetDestination(), disableObstruction))
						{
							newCellEnt = cellLink.GetDestination();

							if (registry.has<Components::Controllable>(blockEnt))
							{
								auto& controllable = registry.get<Components::Controllable>(blockEnt);
								auto& cellLinkDestination = registry.get<Components::Cell>(cellLink.GetDestination());
								controllable.Set(cellLinkDestination.GetParent());
							}
						}
					}
				}
			}
			break;
		case moveDirection_t::WEST:
			if (CanOccupyCell(registry, tempCell.GetWest(), disableObstruction))
			{
				newCellEnt = tempCell.GetWest();
			}
			else
			{
				if (tempCell.GetWest() == entt::null)
				{
					entt::entity cellLinkEnt = GetCellLinkAtCoordinates(registry, coordinate, direction);
					if (cellLinkEnt != entt::null)
					{
						auto& cellLink = registry.get<Components::CellLink>(cellLinkEnt);

						// Only move into the linked cell destination, if we can occupy it.
						if (CanOccupyCell(registry, cellLink.GetDestination(), disableObstruction))
						{
							newCellEnt = cellLink.GetDestination();

							if (registry.has<Components::Controllable>(blockEnt))
							{
								auto& controllable = registry.get<Components::Controllable>(blockEnt);
								auto& cellLinkDestination = registry.get<Components::Cell>(cellLink.GetDestination());
								controllable.Set(cellLinkDestination.GetParent());
							}
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}

	return newCellEnt;
}

// By design, there's only ever going to be zero or one of these.
entt::entity GetActiveControllable(entt::registry& registry)
{
	auto controllableView = registry.view<Components::Controllable>();
	for (auto entity : controllableView)
	{
		auto& controllable = controllableView.get<Components::Controllable>(entity);

		if (controllable.IsEnabled())
		{
			return controllable.Get();
		}
	}

	return entt::null;
}