#include "Utility.h"

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

bool CanOccupyCell(entt::registry& registry, const std::string& containerTag, const entt::entity& cellEntity, const bool& disableObstruction)
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

	auto& tag1 = registry.get<Components::Tag>(cell.GetParent());
	if (containerTag != tag1.Get())
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
			auto& tag = registry.get<Components::Tag>(block.Get()); // We'll be wanting to check which container we're working with later. (eg: Play Area, Hold, Preview, (which play area?))
			if (!tag.IsEnabled() || containerTag != tag.Get())
				continue;

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

const entt::entity& GetCellAtCoordinates2(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate)
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
			// Not really doing anything with container currently.
			// May not be required, or might be covered by some of the checks in coordinate comparisons already.
			auto& tag = registry.get<Components::Tag>(cell.GetParent());
			
			// If we're not looking for this containerTag, this isn't the cell we're looking for. Keep looking.
			if (!tag.IsEnabled() || containerTag != tag.Get())
				continue;

			// The cell we've found shares the same coordinates as what we've specified.
			if (cellCoordinate == coordinate)
			{
				return entity;
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
entt::entity MoveBlockInDirection(entt::registry& registry, const std::string& containerTag, const entt::entity& blockEnt, const moveDirection_t& direction, const unsigned int& distance, const bool& disableObstruction)
{
	auto& coordinate = registry.get<Components::Coordinate>(blockEnt);
	auto& moveable = registry.get<Components::Moveable>(blockEnt);

	entt::entity cellEnt = GetCellAtCoordinates2(registry, containerTag, coordinate);
	if (cellEnt == entt::null)
		return entt::null;

	entt::entity newCellEnt = cellEnt;

	for (int i = 0; i < distance; i++)
	{
		entt::entity tempCellEnt = newCellEnt;
		if (!registry.has<Components::Cell>(tempCellEnt))
			continue;
		Components::Cell& tempCell = registry.get<Components::Cell>(tempCellEnt);

		switch (direction)
		{
		case moveDirection_t::NORTH:
			if (CanOccupyCell(registry, containerTag, tempCell.GetNorth(), disableObstruction))
			{
				newCellEnt = tempCell.GetNorth();
			}
			break;
		case moveDirection_t::SOUTH:
			if (CanOccupyCell(registry, containerTag, tempCell.GetSouth(), disableObstruction))
			{
				newCellEnt = tempCell.GetSouth();
			}
			break;
		case moveDirection_t::EAST:
			if (CanOccupyCell(registry, containerTag, tempCell.GetEast(), disableObstruction))
			{
				newCellEnt = tempCell.GetEast();
			}
			break;
		case moveDirection_t::WEST:
			if (CanOccupyCell(registry, containerTag, tempCell.GetWest(), disableObstruction))
			{
				newCellEnt = tempCell.GetWest();
			}
			break;
		default:
			break;
		}
	}

	return newCellEnt;
}