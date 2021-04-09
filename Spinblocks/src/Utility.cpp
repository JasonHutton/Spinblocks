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

bool IsEntityTetromino(entt::registry& registry, entt::entity ent)
{
	if (ent == entt::null)
		return false;

	return registry.any</*Components::Tetromino, */
		Components::OTetromino, 
		Components::ITetromino,
		Components::TTetromino,
		Components::LTetromino,
		Components::JTetromino,
		Components::STetromino,
		Components::ZTetromino>(ent);
}

Components::Tetromino* GetTetrominoFromEntity(entt::registry& registry, entt::entity entity)
{
	if (entity == entt::null)
		return NULL;

	if (!IsEntityTetromino(registry, entity))
		return NULL;

	if (registry.has<Components::OTetromino>(entity))
		return &registry.get<Components::OTetromino>(entity);

	if (registry.has<Components::ITetromino>(entity))
		return &registry.get<Components::ITetromino>(entity);

	if (registry.has<Components::TTetromino>(entity))
		return &registry.get<Components::TTetromino>(entity);

	if (registry.has<Components::LTetromino>(entity))
		return &registry.get<Components::LTetromino>(entity);

	if (registry.has<Components::JTetromino>(entity))
		return &registry.get<Components::JTetromino>(entity);

	if (registry.has<Components::STetromino>(entity))
		return &registry.get<Components::STetromino>(entity);

	if (registry.has<Components::ZTetromino>(entity))
		return &registry.get<Components::ZTetromino>(entity);

	return NULL;
}

// Take a closer look at this and CanOccupyCell tomorrow....
bool IsAnyBlockInTetrominoObstructingSelf(entt::registry& registry, entt::entity entity, const entt::entity& cellEntity, const bool& disableObstruction)
{
	if (entity == entt::null)
		return false;

	if (!IsEntityTetromino(registry, entity))
		return false;

	Components::Tetromino* tetromino = GetTetrominoFromEntity(registry, entity);
	auto& tmoveable = registry.get<Components::Moveable>(entity);

	auto& cellCoord = registry.get<Components::Coordinate>(cellEntity);
	
	//tetromino->

	//bool bFoundAtLeastOneObstruction = false;
	for (int i = 0; i < 4; i++)
	{
		auto offsetCoordinate = Components::Coordinate(cellCoord.GetParent(),
			(glm::vec2)cellCoord.Get() + tetromino->GetBlockOffsetCoordinates(tetromino->GetCurrentOrientation(), i));



		//auto& block = registry.get<Components::Block>(tetromino->GetBlock(i));
		auto& moveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
		auto& coordinate = registry.get<Components::Coordinate>(tetromino->GetBlock(i));
		//auto& follower = registry.get<Components::Follower>(tetromino->GetBlock(i));

		entt::entity cellEnt = GetCellAtCoordinates2(registry, offsetCoordinate);// coordinate);

		if (!CanOccupyCell(registry, tetromino->GetBlock(i), cellEnt))
			return false;
	}

	return true;

	/*
	
	entt::entity MoveBlockInDirection(entt::registry& registry, const entt::entity& blockEnt, const moveDirection_t& direction, const unsigned int& distance, const bool& disableObstruction)
{
	auto coordinate = registry.get<Components::Coordinate>(blockEnt); // We do NOT want a reference here, we want a copy.
	//auto& moveable = registry.get<Components::Moveable>(blockEnt);

	entt::entity cellEnt = GetCellAtCoordinates2(registry, coordinate);
	if (cellEnt == entt::null)
		return entt::null;

	entt::entity newCellEnt = cellEnt;

	for (unsigned int i = 0; i < distance; i++)
	{
		// We're moving a bunch in this function. Update our current coordiantes while doing so.
		if (i > 0 && registry.has<Components::Coordinate>(newCellEnt))
		{
			const auto& newCoordinate = registry.get<Components::Coordinate>(newCellEnt);
			coordinate.Set(newCoordinate.Get());
			coordinate.SetParent(newCoordinate.GetParent());
		}

		entt::entity tempCellEnt = newCellEnt;
		if (!registry.has<Components::Cell>(tempCellEnt))
			continue;
		const auto& tempCell = registry.get<Components::Cell>(tempCellEnt);

		newCellEnt = MoveBlockInDirection2(registry, blockEnt, direction, coordinate, newCellEnt, tempCell, disableObstruction);
	}

	return newCellEnt;
	
	*/


	/*
	auto followerView = registry.view<Components::Moveable, Components::Coordinate, Components::Follower>();
	for (auto entity : followerView)
	{
		auto& moveable = followerView.get<Components::Moveable>(entity);
		auto& coordinate = followerView.get<Components::Coordinate>(entity);
		auto& follower = followerView.get<Components::Follower>(entity);

		if (moveable.IsEnabled() && coordinate.IsEnabled() && follower.IsEnabled())
		{
			auto& leader = registry.get<Components::OTetromino>(follower.Get()); // FIXME TODO tetromino type issue.
			auto& leaderMoveable = registry.get<Components::Moveable>(follower.Get());

			int i;
			for (i = 0; i < 4; i++)
			{
				if (leader.GetBlock(i) == entity)
				{
					break;
				}
			}

			auto offsetCoordinate = Components::Coordinate(leaderMoveable.GetDesiredCoordinate().GetParent(),
				(glm::vec2)leaderMoveable.GetDesiredCoordinate().Get() + leader.GetBlockOffsetCoordinates(i));

			moveable.SetDesiredCoordinate(offsetCoordinate);
			if (moveable.GetCurrentCoordinate() != moveable.GetDesiredCoordinate())
			{
				// Need to detect if a move is allowed before permitting it.
				coordinate = moveable.GetDesiredCoordinate();
				moveable.SetCurrentCoordinate(coordinate);
			}
		}
	}*/
}

bool CanOccupyCell(entt::registry& registry, const entt::entity& blockEnt, const entt::entity& cellEntity, const bool& disableObstruction)
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

	if (IsEntityTetromino(registry, blockEnt))
	{
		// Might want this conditional on disableObsruction
		return IsAnyBlockInTetrominoObstructingSelf(registry, blockEnt, cellEntity, disableObstruction);
	}

	// Don't check for obstructions. Sooooo, we're able to move into here regardless without further checks.
	if (disableObstruction)
		return true;

	if (AreCoordinatesObstructed(registry, cellCoordinate, blockEnt))
		return false;

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

			auto& blockCoordinate = registry.get<Components::Coordinate>(entity);
			if (blockCoordinate == coordinate)
			{
				// We're short circuiting here on the first match. We probably want to check for container first. (eg: When we've got multiple reference containers in use.)
				return block;
			}
		}
	}

	throw std::runtime_error("Unable to find Block at coordinates!");
}

entt::entity MoveBlockInDirection2(entt::registry& registry, const entt::entity& blockEnt, const moveDirection_t& direction, const Components::Coordinate& coordinate, entt::entity newCellEnt, const Components::Cell& tempCell, const bool& disableObstruction)
{
	if (CanOccupyCell(registry, blockEnt, tempCell.GetDirection(direction), disableObstruction))
	{
		newCellEnt = tempCell.GetDirection(direction);
	}
	else
	{
		// If we can't occupy the cell, because the cell doesn't connect to another cell in this direction...
		if (tempCell.GetDirection(direction) == entt::null)
		{
			// Get a cell link and see if it lets us continue moving in this direction.
			entt::entity cellLinkEnt = GetCellLinkAtCoordinates(registry, coordinate, direction);
			if (cellLinkEnt != entt::null)
			{
				const auto& cellLink = registry.get<Components::CellLink>(cellLinkEnt);

				// Only move into the linked cell destination, if we can occupy it.
				if (CanOccupyCell(registry, blockEnt, cellLink.GetDestination(), disableObstruction))
				{
					newCellEnt = cellLink.GetDestination();

					if (registry.has<Components::Controllable>(blockEnt))
					{
						// Update where we're controlling. We probably want to do this a different way, not in this function. TODO FIXME
						auto& controllable = registry.get<Components::Controllable>(blockEnt);
						const auto& cellLinkDestination = registry.get<Components::Cell>(cellLink.GetDestination());
						controllable.Set(cellLinkDestination.GetParent());
					}
				}
			}
		}
	}

	return newCellEnt;
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
	auto coordinate = registry.get<Components::Coordinate>(blockEnt); // We do NOT want a reference here, we want a copy.
	//auto& moveable = registry.get<Components::Moveable>(blockEnt);

	entt::entity cellEnt = GetCellAtCoordinates2(registry, coordinate);
	if (cellEnt == entt::null)
		return entt::null;

	entt::entity newCellEnt = cellEnt;

	for (unsigned int i = 0; i < distance; i++)
	{
		// We're moving a bunch in this function. Update our current coordiantes while doing so.
		if (i > 0 && registry.has<Components::Coordinate>(newCellEnt))
		{
			const auto& newCoordinate = registry.get<Components::Coordinate>(newCellEnt);
			coordinate.Set(newCoordinate.Get());
			coordinate.SetParent(newCoordinate.GetParent());
		}

		entt::entity tempCellEnt = newCellEnt;
		if (!registry.has<Components::Cell>(tempCellEnt))
			continue;
		const auto& tempCell = registry.get<Components::Cell>(tempCellEnt);

		newCellEnt = MoveBlockInDirection2(registry, blockEnt, direction, coordinate, newCellEnt, tempCell, disableObstruction);
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
			return entity;
		}
	}

	return entt::null;
}

void BuildGrid(entt::registry& registry, const entt::entity& parentEntity)
{
	Components::Container2 container2 = registry.get<Components::Container2>(parentEntity);
	// We want a copy of this to be stored in this scope, because the component reference may change without warning.
	// We could also just store the vector coordinate. Either way.
	Components::Position parentPosition = registry.get<Components::Position>(parentEntity);

	for (unsigned int i = 0; i < container2.GetGridDimensions().x; i++)
	{
		for (unsigned int k = 0; k < container2.GetGridDimensions().y; k++)
		{
			std::string tagName = "Grid";
			tagName += std::to_string(i);
			tagName += "-";
			tagName += std::to_string(k);

			const auto cell = registry.create();
			registry.emplace<Components::Coordinate>(cell, parentEntity, glm::uvec2(i, k));
			registry.emplace<Components::Cell>(cell, parentEntity);
			registry.emplace<Components::Tag>(cell, tagName);
			registry.emplace<Components::Scale>(cell);
			registry.emplace<Components::Position>(cell);
			registry.emplace<Components::DerivePositionFromCoordinates>(cell);// , parentEntity);
			registry.emplace<Components::Renderable>(cell, Components::renderLayer_t::RL_CELL, Model("./data/block/grey.obj"));
			registry.emplace<Components::ScaleToCellDimensions>(cell, parentEntity);
		}
	}

	auto cellView = registry.view<Components::Cell, Components::Coordinate>();
	for (auto entity1 : cellView)
	{
		auto& cell1 = cellView.get<Components::Cell>(entity1);
		auto& coordinate1 = cellView.get<Components::Coordinate>(entity1);
		for (auto entity2 : cellView)
		{
			if (entity1 == entity2)
				continue;

			auto& cell2 = cellView.get<Components::Cell>(entity2);
			auto& coordinate2 = cellView.get<Components::Coordinate>(entity2);

			if (coordinate1.GetParent() != coordinate2.GetParent())
				continue;

			if (coordinate1.Get().y == coordinate2.Get().y)
			{
				if (coordinate1.Get().x + 1 == coordinate2.Get().x)
				{
					cell1.SetEast(entity2);
				}

				if (coordinate1.Get().x - 1 == coordinate2.Get().x)
				{
					cell1.SetWest(entity2);
				}
			}
			if (coordinate1.Get().x == coordinate2.Get().x)
			{
				if (coordinate1.Get().y + 1 == coordinate2.Get().y)
				{
					cell1.SetNorth(entity2);
				}

				if (coordinate1.Get().y - 1 == coordinate2.Get().y)
				{
					cell1.SetSouth(entity2);
				}
			}
		}
	}
}

// We'll want to spawn whole tetrominoes later, not just blocks.
entt::entity SpawnBlock(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, const bool& isControllable)
{
	auto containerView = registry.view<Components::Container2, Components::Tag>();
	for (auto entity : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(entity);
		auto& tag = containerView.get<Components::Tag>(entity); // We'll be wanting to check which container we're working with later. (eg: Play Area, Hold, Preview, (which play area?))
		if (!tag.IsEnabled() || containerTag != tag.Get())
			continue;

		if (container2.IsEnabled() && tag.IsEnabled())
		{
			// Remove all existing controllable blocks.
			// We probably want this more where one locks down, not here, but for now this is fine.
			auto blockView = registry.view<Components::Block, Components::Controllable>();
			for (auto block : blockView)
			{
				registry.remove_if_exists<Components::Controllable>(block);
			}
			Components::Container2 container2 = registry.get<Components::Container2>(entity);
			Components::Position parentPosition = registry.get<Components::Position>(entity);

			const auto piece1 = registry.create();
			registry.emplace<Components::Coordinate>(piece1, spawnCoordinate.GetParent(), spawnCoordinate.Get());
			registry.emplace<Components::Position>(piece1);
			registry.emplace<Components::DerivePositionFromCoordinates>(piece1);
			registry.emplace<Components::Scale>(piece1, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(piece1, Components::renderLayer_t::RL_BLOCK, Model("./data/block/yellow.obj"));
			registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), registry.get<Components::Coordinate>(piece1));
			//registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), Components::Coordinate(glm::uvec2(1, 0)));// registry.get<Components::Coordinate>(piece1));
			if (isControllable)
			{
				registry.emplace<Components::Controllable>(piece1, entity);
			}
			registry.emplace<Components::Block>(piece1, entity);
			registry.emplace<Components::Obstructable>(piece1, entity);
			registry.emplace<Components::Obstructs>(piece1);

			// Temporary for testing. Switch directly to the falling state.
			if (registry.has<Components::Moveable>(piece1))
			{
				auto& moveable = registry.get<Components::Moveable>(piece1);
				moveable.SetMovementState(Components::movementStates_t::FALL);
			}

			return piece1;
		}
	}

	return entt::null;
}

entt::entity SpawnFollowerBlock(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, entt::entity followedEntity, const std::string& blockModelPath)
{
	auto containerView = registry.view<Components::Container2, Components::Tag>();
	for (auto entity : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(entity);
		auto& tag = containerView.get<Components::Tag>(entity); // We'll be wanting to check which container we're working with later. (eg: Play Area, Hold, Preview, (which play area?))
		if (!tag.IsEnabled() || containerTag != tag.Get())
			continue;

		if (container2.IsEnabled() && tag.IsEnabled())
		{
			// Remove all existing controllable blocks.
			// We probably want this more where one locks down, not here, but for now this is fine.
			auto blockView = registry.view<Components::Block, Components::Controllable>();
			for (auto block : blockView)
			{
				registry.remove_if_exists<Components::Controllable>(block);
			}
			Components::Container2 container2 = registry.get<Components::Container2>(entity);
			Components::Position parentPosition = registry.get<Components::Position>(entity);

			const auto piece1 = registry.create();
			registry.emplace<Components::Coordinate>(piece1, spawnCoordinate.GetParent(), spawnCoordinate.Get());
			registry.emplace<Components::Position>(piece1);
			registry.emplace<Components::DerivePositionFromCoordinates>(piece1);
			registry.emplace<Components::Scale>(piece1, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(piece1, Components::renderLayer_t::RL_BLOCK, Model(blockModelPath));
			registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), registry.get<Components::Coordinate>(piece1));
			//registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), Components::Coordinate(glm::uvec2(1, 0)));// registry.get<Components::Coordinate>(piece1));

			registry.emplace<Components::Block>(piece1, entity);
			registry.emplace<Components::Obstructable>(piece1, entity);
			registry.emplace<Components::Obstructs>(piece1);
			registry.emplace<Components::Follower>(piece1, followedEntity);

			// Temporary for testing. Switch directly to the falling state.
			if (registry.has<Components::Moveable>(piece1))
			{
				auto& moveable = registry.get<Components::Moveable>(piece1);
				moveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}

			return piece1;
		}
	}

	return entt::null;
}

void LinkCoordinates(entt::registry& registry, const Components::Coordinate& origin, const Components::Coordinate& destination, const moveDirection_t& moveDir, const moveDirection_t& moveDirReverse)
{
	auto originCoordView = registry.view<Components::Cell, Components::Coordinate>();
	auto destinationCoordView = registry.view<Components::Cell, Components::Coordinate>();
	for (auto originEnt : originCoordView)
	{
		const auto originCoord = originCoordView.get<Components::Coordinate>(originEnt);
		const auto originCell = originCoordView.get<Components::Cell>(originEnt);

		if (originCoord.Get() != origin.Get())
			continue;

		if (originCoord.GetParent() != origin.GetParent())
			continue;

		for (auto destinationEnt : destinationCoordView)
		{
			if (originEnt == destinationEnt)
				continue;

			const auto destinationCoord = destinationCoordView.get<Components::Coordinate>(destinationEnt);
			const auto destinationCell = destinationCoordView.get<Components::Cell>(destinationEnt);

			if (destinationCoord.Get() != destination.Get())
				continue;

			if (destinationCoord.GetParent() != destination.GetParent())
				continue;

			const auto marker1 = registry.create();
			registry.emplace<Components::CellLink>(marker1, originEnt, destinationEnt, moveDir);

			registry.emplace<Components::Coordinate>(marker1, originCoord.GetParent(), originCoord.Get());
			registry.emplace<Components::Position>(marker1);
			registry.emplace<Components::DerivePositionFromCoordinates>(marker1);// , originCoord.GetParent());

			if (registry.has<Components::Container2>(originCell.GetParent()))
			{
				auto& container = registry.get<Components::Container2>(originCoord.GetParent());
				registry.emplace<Components::Scale>(marker1, container.GetCellDimensions3());
			}
			registry.emplace<Components::Renderable>(marker1, Components::renderLayer_t::RL_MARKER_UNDER, Model("./data/block/green.obj"));
		}
	}
}

entt::entity SpawnTetromino(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, const tetrominoType_t& tetrominoType, const bool& isControllable)
{
	auto controllableView = registry.view<Components::Controllable>();
	for (auto controllable : controllableView)
	{
		registry.remove_if_exists<Components::Controllable>(controllable);
	}

	const auto tetrominoEnt = registry.create();

	registry.emplace<Components::Coordinate>(tetrominoEnt, spawnCoordinate.GetParent(), spawnCoordinate.Get());
	registry.emplace<Components::Position>(tetrominoEnt);

	switch (tetrominoType)
	{
	case tetrominoType_t::O:
		registry.emplace<Components::OTetromino>(tetrominoEnt);
	case tetrominoType_t::I:
		registry.emplace<Components::ITetromino>(tetrominoEnt);
		break;
	case tetrominoType_t::T:
		registry.emplace<Components::TTetromino>(tetrominoEnt);
		break;
	case tetrominoType_t::L:
		registry.emplace<Components::LTetromino>(tetrominoEnt);
		break;
	case tetrominoType_t::J:
		registry.emplace<Components::JTetromino>(tetrominoEnt);
		break;
	case tetrominoType_t::S:
		registry.emplace<Components::STetromino>(tetrominoEnt);
		break;
	case tetrominoType_t::Z:
		registry.emplace<Components::ZTetromino>(tetrominoEnt);
		break;
	default:
		assert(false);
	}

	auto* tetromino = GetTetrominoFromEntity(registry, tetrominoEnt);
	
	switch (tetromino->GetType())
	{
	case tetrominoType_t::I:
		registry.emplace<Components::DerivePositionFromCoordinates>(tetrominoEnt, entt::null, glm::vec2(cellWidth / 2, -(static_cast<int>(cellHeight) / 2)));
		break;
	default:
		registry.emplace<Components::DerivePositionFromCoordinates>(tetrominoEnt);
		break;
	}

	registry.emplace<Components::Scale>(tetrominoEnt, glm::uvec2(cellWidth * tetromino->GetPatternWidth(), cellHeight * tetromino->GetPatternHeight()));
	registry.emplace<Components::Container2>(tetrominoEnt, glm::uvec2(tetromino->GetPatternWidth(), tetromino->GetPatternHeight()), glm::uvec2(cellWidth, cellHeight));

	for (int i = 0; i < 4; i++)
	{
		auto spawnPoint = Components::Coordinate(spawnCoordinate.GetParent(),
			(glm::vec2)spawnCoordinate.Get() + tetromino->GetBlockOffsetCoordinates(tetromino->GetCurrentOrientation(), i));

		entt::entity blockEnt = SpawnFollowerBlock(registry, containerTag, spawnPoint, tetrominoEnt, tetromino->GetBlockModelPath());
		tetromino->AddBlock(blockEnt);
	}

	int width = tetromino->GetPatternWidth();
	string modelpath = tetromino->GetBlockModelPath();

	if (isControllable)
	{
		registry.emplace<Components::Controllable>(tetrominoEnt, spawnCoordinate.GetParent());
	}
	//registry.emplace<Components::Renderable>(tetrominoEnt, Components::renderLayer_t::RL_TETROMINO, Model("./data/block/purple.obj"));
	registry.emplace<Components::Moveable>(tetrominoEnt, registry.get<Components::Coordinate>(tetrominoEnt), registry.get<Components::Coordinate>(tetrominoEnt));
	registry.emplace<Components::Obstructable>(tetrominoEnt, spawnCoordinate.GetParent());

	// Temporary for testing. Switch directly to the falling state.
	if (registry.has<Components::Moveable>(tetrominoEnt))
	{
		auto& moveable = registry.get<Components::Moveable>(tetrominoEnt);
		moveable.SetMovementState(Components::movementStates_t::FALL);
	}

	return tetrominoEnt;
}

// See if the coordinates specified will obstruct the probe entity. (The probe entity might not be checking for obstructions, or similar.)
bool AreCoordinatesObstructed(entt::registry& registry, const Components::Coordinate& coordinate, const entt::entity probeEntity)
{
	if (probeEntity == entt::null)
		return false;

	if (!registry.has<Components::Obstructable>(probeEntity))
		return false;

	const auto& probeObstructable = registry.get<Components::Obstructable>(probeEntity);
	if (!probeObstructable.IsEnabled())
		return false;

	// Go through all the obstructing entities with these coordinates. Any positive match will obstruct.
	auto coordinateView = registry.view<Components::Coordinate, Components::Obstructs>();
	for (auto entity : coordinateView)
	{
		const auto& obstructs = coordinateView.get<Components::Obstructs>(entity);
		const auto& obstructsCoordinate = coordinateView.get<Components::Coordinate>(entity);

		// Don't self-obstruct
		if(entity == probeEntity)
			continue;

		// We only care about matching coordinates.
		if (obstructsCoordinate != coordinate)
			continue;

		if (!obstructsCoordinate.IsEnabled() || !obstructs.IsEnabled())
			continue;

		if (registry.has<Components::Follower>(entity) && registry.has<Components::Follower>(probeEntity))
		{
			const auto& probeFollower = registry.get<Components::Follower>(probeEntity);
			const auto& obstructsFollower = registry.get<Components::Follower>(entity);

			if (probeFollower.Get() != obstructsFollower.Get())
			{ // Followers are not the same. Obstruct.
				return true;
			}
		}
		else
		{ // One or both are not followers. Obstruct.
			return true;
		}
	}

	return false;
}

glm::uvec2 GetTetrominoSpawnCoordinates(entt::registry& registry, const entt::entity entity)
{
	if (entity == entt::null)
		throw std::runtime_error("Entity is null!");

	if (!IsEntityTetromino(registry, entity))
		throw std::runtime_error("Entity is not a tetromino!");

	auto* tetromino = GetTetrominoFromEntity(registry, entity);

	return GetTetrominoSpawnCoordinates(tetromino->GetType());
}

glm::uvec2 GetTetrominoSpawnCoordinates(const tetrominoType_t& type)
{
	switch (type)
	{
	case tetrominoType_t::T:
	case tetrominoType_t::L:
	case tetrominoType_t::J:
	case tetrominoType_t::S:
	case tetrominoType_t::Z:
		return glm::uvec2(4, 17);
		break;
	case tetrominoType_t::I:
		return glm::uvec2(4, 18);
	case tetrominoType_t::O:
		return glm::uvec2(4, 17);
		break;
	default:
		throw std::runtime_error("Invalid tetrominoType_t!");
	}

	assert(false);
}