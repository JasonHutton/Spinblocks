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

entt::entity FindEntityByTag(entt::registry& registry, const std::string& tagName)
{
	return cachedTagLookup.Get(registry, tagName);
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

const std::string GetTagOfEntity(entt::registry& registry, const entt::entity& entity)
{
	if (registry.all_of<Components::Tag>(entity))
	{
		auto& tag = registry.get<Components::Tag>(entity);
		if (tag.IsEnabled())
		{
			return tag.Get();
		}
	}

	return "";
}

bool IsEntityTetromino(entt::registry& registry, entt::entity ent)
{
	if (ent == entt::null)
		return false;

	if (!registry.valid(ent)) // This should never happen, yet it is. FIXME TODO
		return false;

	return registry.any_of</*Components::Tetromino, */
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

	if (registry.all_of<Components::OTetromino>(entity))
		return &registry.get<Components::OTetromino>(entity);

	if (registry.all_of<Components::ITetromino>(entity))
		return &registry.get<Components::ITetromino>(entity);

	if (registry.all_of<Components::TTetromino>(entity))
		return &registry.get<Components::TTetromino>(entity);

	if (registry.all_of<Components::LTetromino>(entity))
		return &registry.get<Components::LTetromino>(entity);

	if (registry.all_of<Components::JTetromino>(entity))
		return &registry.get<Components::JTetromino>(entity);

	if (registry.all_of<Components::STetromino>(entity))
		return &registry.get<Components::STetromino>(entity);

	if (registry.all_of<Components::ZTetromino>(entity))
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
		if (i > 0 && registry.all_of<Components::Coordinate>(newCellEnt))
		{
			const auto& newCoordinate = registry.get<Components::Coordinate>(newCellEnt);
			coordinate.Set(newCoordinate.Get());
			coordinate.SetParent(newCoordinate.GetParent());
		}

		entt::entity tempCellEnt = newCellEnt;
		if (!registry.all_of<Components::Cell>(tempCellEnt))
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

bool IsAnyBlockInTetrominoObstructed(entt::registry& registry, entt::entity entity)
{
	if (entity == entt::null)
		return false;

	if (!IsEntityTetromino(registry, entity))
		return false;

	auto& tetCoord = registry.get<Components::Coordinate>(entity);

	Components::Tetromino* tetromino = GetTetrominoFromEntity(registry, entity);

	for (int i = 0; i < 4; i++)
	{
		auto& coordinate = registry.get<Components::Coordinate>(tetromino->GetBlock(i));
		if (AreCoordinatesObstructed(registry, coordinate, tetromino->GetBlock(i)))
			return true;
	}

	return false;
}

bool CanOccupyCell(entt::registry& registry, const entt::entity& blockEnt, const entt::entity& cellEntity, const bool& disableObstruction)
{
	if (cellEntity == entt::null)
		return false;

	if (!registry.all_of<Components::Cell>(cellEntity))
		return false;

	if (!registry.all_of<Components::Coordinate>(cellEntity))
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

	if (!registry.all_of<Components::Coordinate>(entity))
		throw std::runtime_error("Entity does not have component!");

	return registry.get<Components::Coordinate>(entity);
}

const Components::Cell& GetCellOfEntity(entt::registry& registry, const entt::entity& entity)
{
	if (entity == entt::null)
		throw std::runtime_error("Entity is NULL!");

	if (!registry.all_of<Components::Cell>(entity))
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
			if (!registry.all_of<Components::Container2>(cell.GetParent()))
				continue;
			if (!registry.all_of<Components::Tag>(cell.GetParent()))
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
			if (registry.all_of<Components::Cell>(cellLink.GetSource()))
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

					if (registry.all_of<Components::Controllable>(blockEnt))
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
		if (i > 0 && registry.all_of<Components::Coordinate>(newCellEnt))
		{
			const auto& newCoordinate = registry.get<Components::Coordinate>(newCellEnt);
			coordinate.Set(newCoordinate.Get());
			coordinate.SetParent(newCoordinate.GetParent());
		}

		entt::entity tempCellEnt = newCellEnt;
		if (!registry.all_of<Components::Cell>(tempCellEnt))
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

void PlaceCensor(entt::registry& registry, const Components::Coordinate& coordinate, const bool& startVisible, const bool& directional, const std::vector<moveDirection_t> directions)
{
	auto containerView = registry.view<Components::Container2>();
	for (auto containerEnt : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(containerEnt);

		if (container2.IsEnabled())
		{
			if (containerEnt != coordinate.GetParent())
				continue;

			entt::entity cellEnt = GetCellAtCoordinates2(registry, coordinate);

			if (cellEnt == entt::null)
				continue;

			const auto censor = registry.create();
			registry.emplace<Components::Censor>(censor);
			registry.emplace<Components::Coordinate>(censor, coordinate.GetParent(), coordinate.Get());
			registry.emplace<Components::Position>(censor);
			registry.emplace<Components::DerivePositionFromCoordinates>(censor);
			registry.emplace<Components::Scale>(censor, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(censor, Components::renderLayer_t::RL_MARKER_OVER, Model("./data/block/grey.obj"), startVisible);
			registry.emplace<Components::Orientation>(censor);
			registry.emplace<Components::ReferenceEntity>(censor, coordinate.GetParent());
			if (directional)
			{
				registry.emplace<Components::DirectionallyActive>(censor, directions);
			}
		}
	}
}

void FillPauseCensors(entt::registry&  registry, entt::entity matrix, entt::entity bagArea)
{
	Components::Container2 container1 = registry.get<Components::Container2>(matrix);
	for (unsigned int i = BufferAreaDepth; i < container1.GetGridDimensions().x - BufferAreaDepth; i++)
	{
		for (unsigned int k = BufferAreaDepth; k < container1.GetGridDimensions().y - BufferAreaDepth; k++)
		{
			PlaceCensor(registry, Components::Coordinate(matrix, glm::vec2(i, k)), false, false, std::vector<moveDirection_t>());
		}
	}

	// Directional censors for the top buffer area of the matrix here. FIXME TODO
	// This is basically just directional walls' positioning loops(different, but same idea) using censors instead.

	Components::Container2 container2 = registry.get<Components::Container2>(bagArea);
	for (unsigned int i = 0; i < container2.GetGridDimensions().x; i++)
	{
		for (unsigned int k = 0; k < container2.GetGridDimensions().y; k++)
		{
			PlaceCensor(registry, Components::Coordinate(bagArea, glm::vec2(i, k)), false, false, std::vector<moveDirection_t>());
		}
	}
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
			std::string tagName = GetTagOfEntity(registry, parentEntity);
			tagName += ":Grid:";
			tagName += std::to_string(i);
			tagName += "-";
			tagName += std::to_string(k);

			const auto cell = registry.create();
			registry.emplace<Components::Coordinate>(cell, parentEntity, glm::uvec2(i, k));
			registry.emplace<Components::Cell>(cell, parentEntity);
			registry.emplace<Components::Tag>(cell, tagName);
			registry.emplace<Components::Scale>(cell, container2.GetCellDimensions3());
			registry.emplace<Components::Position>(cell);
			registry.emplace<Components::DerivePositionFromCoordinates>(cell, parentEntity);
			//registry.emplace<Components::Renderable>(cell, Components::renderLayer_t::RL_CELL, Model("./data/block/darkgrey.obj"));
			//registry.emplace<Components::ScaleToCellDimensions>(cell, parentEntity);
			registry.emplace<Components::Orientation>(cell);
			registry.emplace<Components::ReferenceEntity>(cell, parentEntity);
			registry.emplace<Components::InheritScalingFromParent>(cell, false);
			//registry.emplace<Components::DerivePositionFromParent>(cell, parentEntity);
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
			Components::Container2 container2 = registry.get<Components::Container2>(entity);
			Components::Position parentPosition = registry.get<Components::Position>(entity);

			const auto piece1 = registry.create();
			registry.emplace<Components::Coordinate>(piece1, spawnCoordinate.GetParent(), spawnCoordinate.Get());
			registry.emplace<Components::Position>(piece1);
			registry.emplace<Components::DerivePositionFromCoordinates>(piece1, entity);
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

			registry.emplace<Components::Orientation>(piece1);
			registry.emplace<Components::ReferenceEntity>(piece1, entity);

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

			registry.emplace<Components::Orientation>(piece1);
			registry.emplace<Components::ReferenceEntity>(piece1, entity);

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

			if (registry.all_of<Components::Container2>(originCell.GetParent()))
			{
				auto& container = registry.get<Components::Container2>(originCoord.GetParent());
				registry.emplace<Components::Scale>(marker1, container.GetCellDimensions3());
			}
			registry.emplace<Components::Renderable>(marker1, Components::renderLayer_t::RL_MARKER_UNDER, Model("./data/block/green.obj"));
		}
	}
}

void RelocateBlock(entt::registry& registry, const Components::Coordinate& newCoordinate, entt::entity blockEnt)
{
	if (blockEnt == entt::null)
		return;

	auto& coordinate = registry.get<Components::Coordinate>(blockEnt);
	coordinate.Set(newCoordinate.Get());
	coordinate.SetParent(newCoordinate.GetParent());

	auto& derivePositionFromCoordinates = registry.get<Components::DerivePositionFromCoordinates>(blockEnt);
	derivePositionFromCoordinates.Set(newCoordinate.GetParent());

	// Not going to adjust cell dimensions for now. They're not changing in this, though hypothetically they COULD. FIXME TODO

	auto& moveable = registry.get<Components::Moveable>(blockEnt);
	moveable.SetCurrentCoordinate(newCoordinate);
	moveable.SetDesiredCoordinate(moveable.GetCurrentCoordinate());

	auto& block = registry.get<Components::Block>(blockEnt);
	block.Set(newCoordinate.GetParent());

	auto& obstructable = registry.get<Components::Obstructable>(blockEnt);
	obstructable.Set(newCoordinate.GetParent());

	// This is wrong, we don't want to be "following" the matrix we're in. Should be fine untouched.
	/*if (registry.all_of<Components::Follower>(blockEnt))
	{
		auto& follower = registry.get<Components::Follower>(blockEnt);
		follower.Set(newCoordinate.GetParent());
	}*/

	auto& referenceEntity = registry.get<Components::ReferenceEntity>(blockEnt);
	referenceEntity.Set(newCoordinate.GetParent());
}

void RelocateTetromino(entt::registry& registry, const Components::Coordinate& newCoordinate, entt::entity tetrominoEnt)
{
	if (tetrominoEnt == entt::null)
		return;

	if (!IsEntityTetromino(registry, tetrominoEnt))
		return;

	auto* tetromino = GetTetrominoFromEntity(registry, tetrominoEnt);
	auto& coordinate = registry.get<Components::Coordinate>(tetrominoEnt);
	coordinate.Set(newCoordinate.Get());
	coordinate.SetParent(newCoordinate.GetParent());

	// Not going to adjust cell dimensions for now. They're not changing in this, though hypothetically they COULD. FIXME TODO

	for (int i = 0; i < 4; i++)
	{
		auto spawnPoint = Components::Coordinate(newCoordinate.GetParent(),
			(glm::vec2)newCoordinate.Get() + tetromino->GetBlockOffsetCoordinates(tetromino->GetCurrentOrientation(), i));
		RelocateBlock(registry, spawnPoint, tetromino->GetBlock(i));
	}

	auto& moveable = registry.get<Components::Moveable>(tetrominoEnt);
	moveable.SetCurrentCoordinate(newCoordinate);
	moveable.SetDesiredCoordinate(moveable.GetCurrentCoordinate());

	auto& obstructable = registry.get<Components::Obstructable>(tetrominoEnt);
	obstructable.Set(newCoordinate.GetParent());
}

moveDirection_t GetDesiredDirectionOfTetromino(entt::registry& registry, const entt::entity& containerEnt)
{
	moveDirection_t desiredDirection = moveDirection_t::NORTH;

	if (registry.all_of<Components::CardinalDirection>(containerEnt))
	{
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(containerEnt);

		desiredDirection = playAreaDirection.GetCurrentOrientation();
	}
	else if (registry.all_of<Components::ReferenceEntity>(containerEnt))
	{
		auto& playAreaRefEnt = registry.get<Components::ReferenceEntity>(containerEnt);
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaRefEnt.Get());

		desiredDirection = playAreaDirection.GetCurrentOrientation();
	}

	return desiredDirection;
}

entt::entity SpawnProjectedFollowerBlock(entt::registry& registry, const Components::Coordinate& spawnCoordinate, entt::entity followedEntity, entt::entity parentTetromino, const std::string& blockModelPath)
{
	const auto& container = registry.get<Components::Container2>(spawnCoordinate.GetParent());

	if (container.IsEnabled())
	{
		const auto piece1 = registry.create();
		registry.emplace<Components::Coordinate>(piece1, spawnCoordinate.GetParent(), spawnCoordinate.Get());
		registry.emplace<Components::Position>(piece1);
		registry.emplace<Components::DerivePositionFromCoordinates>(piece1);
		registry.emplace<Components::Scale>(piece1, container.GetCellDimensions3());
		registry.emplace<Components::Renderable>(piece1, Components::renderLayer_t::RL_BLOCK, Model(blockModelPath));
		registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), registry.get<Components::Coordinate>(piece1));
		//registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), Components::Coordinate(glm::uvec2(1, 0)));// registry.get<Components::Coordinate>(piece1));

		registry.emplace<Components::Block>(piece1, spawnCoordinate.GetParent());
		registry.emplace<Components::Obstructable>(piece1, spawnCoordinate.GetParent());
		registry.emplace<Components::Follower>(piece1, followedEntity);

		registry.emplace<Components::Orientation>(piece1);
		registry.emplace<Components::ReferenceEntity>(piece1, spawnCoordinate.GetParent());
		registry.emplace<Components::ProjectionOf>(piece1, parentTetromino);

		return piece1;
	}

	return entt::null;
}

entt::entity SpawnProjectedTetromino(entt::registry& registry, const entt::entity& tetrominoEnt)
{
	if (!IsEntityTetromino(registry, tetrominoEnt))
		return entt::null;

	auto* tetromino = GetTetrominoFromEntity(registry, tetrominoEnt);
	auto tetCoord = registry.get<Components::Coordinate>(tetrominoEnt);
	moveDirection_t currentOrientation = tetromino->GetCurrentOrientation();
	tetrominoType_t tetType = tetromino->GetType();
	int patternWidth = tetromino->GetPatternWidth();
	int pattternHeight = tetromino->GetPatternHeight();
		
	const auto projectionEnt = registry.create();

	registry.emplace<Components::Coordinate>(projectionEnt, tetCoord.GetParent(), tetCoord.Get());
	registry.emplace<Components::Position>(projectionEnt);

	switch (tetType)
	{
	case tetrominoType_t::O:
		registry.emplace<Components::OTetromino>(projectionEnt, currentOrientation);
		break;
	case tetrominoType_t::I:
		registry.emplace<Components::ITetromino>(projectionEnt, currentOrientation);
		break;
	case tetrominoType_t::T:
		registry.emplace<Components::TTetromino>(projectionEnt, currentOrientation);
		break;
	case tetrominoType_t::L:
		registry.emplace<Components::LTetromino>(projectionEnt, currentOrientation);
		break;
	case tetrominoType_t::J:
		registry.emplace<Components::JTetromino>(projectionEnt, currentOrientation);
		break;
	case tetrominoType_t::S:
		registry.emplace<Components::STetromino>(projectionEnt, currentOrientation);
		break;
	case tetrominoType_t::Z:
		registry.emplace<Components::ZTetromino>(projectionEnt, currentOrientation);
		break;
	default:
		assert(false);
	}

	//GetDesiredDirectionOfTetromino(registry, originCoordinate.GetParent());

	switch (tetType)
	{
	case tetrominoType_t::I:
		registry.emplace<Components::DerivePositionFromCoordinates>(projectionEnt, entt::null, glm::vec2(cellWidth / 2, -(static_cast<int>(cellHeight) / 2)));
		break;
	default:
		registry.emplace<Components::DerivePositionFromCoordinates>(projectionEnt);
		break;
	}

	registry.emplace<Components::Scale>(projectionEnt, glm::uvec2(cellWidth * patternWidth, cellHeight * pattternHeight));
	registry.emplace<Components::Container2>(projectionEnt, glm::uvec2(patternWidth, pattternHeight), glm::uvec2(cellWidth, cellHeight));

	auto* projection = GetTetrominoFromEntity(registry, projectionEnt);
	auto projCoord = registry.get<Components::Coordinate>(projectionEnt);
	for (int i = 0; i < 4; i++)
	{
		auto spawnPoint = Components::Coordinate(projCoord.GetParent(),
			(glm::vec2)projCoord.Get() + projection->GetBlockOffsetCoordinates(currentOrientation, i));

		entt::entity blockEnt = SpawnProjectedFollowerBlock(registry, spawnPoint, projectionEnt, tetrominoEnt, "./data/block/purple.obj");
		projection->AddBlock(blockEnt);
	}
	
	//registry.emplace<Components::Renderable>(tetrominoEnt, Components::renderLayer_t::RL_TETROMINO, Model("./data/block/purple.obj"));
	registry.emplace<Components::Moveable>(projectionEnt, projCoord, projCoord);
	registry.emplace<Components::Obstructable>(projectionEnt, projCoord.GetParent());
	registry.emplace<Components::ProjectionOf>(projectionEnt, tetrominoEnt);

	return projectionEnt;
}

entt::entity SpawnTetromino(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, const tetrominoType_t& tetrominoType, const bool& isControllable)
{
	const auto tetrominoEnt = registry.create();

	registry.emplace<Components::Coordinate>(tetrominoEnt, spawnCoordinate.GetParent(), spawnCoordinate.Get());
	registry.emplace<Components::Position>(tetrominoEnt);

	moveDirection_t currentDirection = moveDirection_t::NORTH;
	if (registry.all_of<Components::CardinalDirection>(spawnCoordinate.GetParent()))
	{
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(spawnCoordinate.GetParent());

		currentDirection = playAreaDirection.GetCurrentOrientation();
	}
	else if (registry.all_of<Components::ReferenceEntity>(spawnCoordinate.GetParent()))
	{
		auto& playAreaRefEnt = registry.get<Components::ReferenceEntity>(spawnCoordinate.GetParent());
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaRefEnt.Get());

		currentDirection = playAreaDirection.GetCurrentOrientation();
	}

	switch (tetrominoType)
	{
	case tetrominoType_t::O:
		registry.emplace<Components::OTetromino>(tetrominoEnt, currentDirection);
		break;
	case tetrominoType_t::I:
		registry.emplace<Components::ITetromino>(tetrominoEnt, currentDirection);
		break;
	case tetrominoType_t::T:
		registry.emplace<Components::TTetromino>(tetrominoEnt, currentDirection);
		break;
	case tetrominoType_t::L:
		registry.emplace<Components::LTetromino>(tetrominoEnt, currentDirection);
		break;
	case tetrominoType_t::J:
		registry.emplace<Components::JTetromino>(tetrominoEnt, currentDirection);
		break;
	case tetrominoType_t::S:
		registry.emplace<Components::STetromino>(tetrominoEnt, currentDirection);
		break;
	case tetrominoType_t::Z:
		registry.emplace<Components::ZTetromino>(tetrominoEnt, currentDirection);
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

	return tetrominoEnt;
}

// See if the coordinates specified will obstruct the probe entity. (The probe entity might not be checking for obstructions, or similar.)
bool AreCoordinatesObstructed(entt::registry& registry, const Components::Coordinate& coordinate, const entt::entity probeEntity)
{
	if (probeEntity == entt::null)
		return false;

	if (!registry.all_of<Components::Obstructable>(probeEntity))
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

		if (registry.all_of<Components::ProjectionOf>(probeEntity))
		{
			const auto& probeProjectionOf = registry.get<Components::ProjectionOf>(probeEntity);
			if (probeProjectionOf.IsEnabled())
			{
				if (probeProjectionOf.Get() == entity)
				{ // We only care if we're not a projection of this obstruction
					continue;
				}

				if (registry.all_of<Components::Follower>(entity))
				{
					const auto& obstructsFollower = registry.get<Components::Follower>(entity);

					if (probeProjectionOf.Get() == obstructsFollower.Get())
					{
						continue;
					}
				}
			}
		}

		if (registry.all_of<Components::Follower>(entity) && registry.all_of<Components::Follower>(probeEntity))
		{
			const auto& probeFollower = registry.get<Components::Follower>(probeEntity);
			const auto& obstructsFollower = registry.get<Components::Follower>(entity);

			if (!probeFollower.IsEnabled() || !obstructsFollower.IsEnabled())
			{
				// At least one follower isn't enabled. Obstruct.
				return true;
			}

			if (probeFollower.Get() == entt::null || obstructsFollower.Get() == entt::null)
			{
				// At least one follower, isn't following anything. Obstruct.
				return true;
			}

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


glm::uvec2 GetTetrominoSpawnCoordinates(entt::registry& registry, const std::string& containerTag, const tetrominoType_t& tetrominoType)
{
	entt::entity foundEntity = entt::null;

	entt::entity containerEntity = FindContainerEntityByTag(registry, containerTag);

	auto& container = registry.get<Components::Container2>(containerEntity);

	moveDirection_t currentDirection = moveDirection_t::NORTH;
	if (registry.all_of<Components::CardinalDirection>(containerEntity))
	{
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(containerEntity);

		currentDirection = playAreaDirection.GetCurrentOrientation();
	}
	else if(registry.all_of<Components::ReferenceEntity>(containerEntity))
	{
		auto& playAreaRefEnt = registry.get<Components::ReferenceEntity>(containerEntity);
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaRefEnt.Get());

		currentDirection = playAreaDirection.GetCurrentOrientation();
	}

	auto spawnMarkerView = registry.view<Components::SpawnMarker, Components::Coordinate>();
	for (auto entity : spawnMarkerView)
	{
		auto& marker = spawnMarkerView.get<Components::SpawnMarker>(entity);
		auto& coordinate = spawnMarkerView.get<Components::Coordinate>(entity);

		if (coordinate.GetParent() != containerEntity)
			continue;

		if (!marker.ValidForTetrominoType(tetrominoType))
			continue;

		if (registry.all_of<Components::DirectionallyActive>(entity))
		{
			auto& dirActive = registry.get<Components::DirectionallyActive>(entity);
			if (dirActive.IsActive(currentDirection))
			{
				return coordinate.Get();
			}
		}
		else
		{
			return coordinate.Get();
		}
	}
	
	throw std::runtime_error("Unable to find valid spawn marker!");
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

glm::mat4 GetModelMatrixOfEntity(entt::registry& registry, entt::entity entity, const bool& inheritScaling, const bool& childCall)
{
	if (!registry.all_of<Components::Position, Components::Orientation, Components::Scale>(entity))
		throw std::runtime_error("Missing a required component to get a model matrix!");

	auto& position = registry.get<Components::Position>(entity);
	auto& orientation = registry.get<Components::Orientation>(entity);
	auto& scale = registry.get<Components::Scale>(entity);

	entt::entity parentEnt = entt::null;
	if (registry.all_of<Components::ReferenceEntity>(entity))
	{
		auto& parent = registry.get<Components::ReferenceEntity>(entity);
		parentEnt = parent.Get();
	}
	bool shouldInheritScaling = false;
	if (registry.all_of<Components::InheritScalingFromParent>(entity))
	{
		auto& inheritScalingFromParent = registry.get<Components::InheritScalingFromParent>(entity);
		shouldInheritScaling = inheritScalingFromParent.Get();
	}

	glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity Matrix

	if(parentEnt != entt::null)
	{
		if (registry.all_of<Components::Position, Components::Orientation, Components::Scale>(parentEnt))
		{
			modelMatrix = GetModelMatrixOfEntity(registry, parentEnt, shouldInheritScaling, true);
		}
	}

	modelMatrix = glm::translate(modelMatrix, position.Get());
		
	glm::mat4 rotationMatrix = glm::mat4(1.0f);

	rotationMatrix = glm::rotate(rotationMatrix, orientation.Get(), orientation.GetAxis());

	// Look at this, this might be redundant with GLM functions.
	modelMatrix = modelMatrix * rotationMatrix;

	if ((childCall && inheritScaling) || !childCall)
	{
		// scale
		modelMatrix = glm::scale(modelMatrix, scale.Get());
	}

	return modelMatrix;
}

void LinkNodes(entt::registry& registry, Components::NodeOrder& nodeOrder, entt::entity source, entt::entity destination)
{
	entt::entity sourceNodeEnt = source;
	entt::entity destinationNodeEnt = nodeOrder.GetNode();

	while (destinationNodeEnt != entt::null)
	{
		auto& node = registry.get<Components::QueueNode>(destinationNodeEnt);
		destinationNodeEnt = nodeOrder.GetNode();
		node.SetSource(sourceNodeEnt);
		node.SetDestination(destinationNodeEnt);

		sourceNodeEnt = node.GetSelf();
	}

	auto& node = registry.get<Components::QueueNode>(sourceNodeEnt);
	node.SetDestination(destination);
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

void RotatePlayArea(entt::registry& registry, const rotationDirection_t& rotationDirection)
{
	if (rotationDirection == rotationDirection_t::NONE)
		return;

	auto playAreaView = registry.view<Components::Tag, Components::CardinalDirection, Components::Orientation>();
	for (auto entity : playAreaView)
	{
		auto& tag = playAreaView.get<Components::Tag>(entity);
		auto& cardinalDirection = playAreaView.get<Components::CardinalDirection>(entity);
		auto& orientation = playAreaView.get<Components::Orientation>(entity);

		if (tag.IsEnabled() && cardinalDirection.IsEnabled() && orientation.IsEnabled() && tag.Get() == GetTagFromContainerType(containerType_t::PLAY_AREA))
		{
			auto& playAreaCardinalDirection = registry.get<Components::CardinalDirection>(entity);

			// Quick and dirty, rather than handling through a system. Refactor later. FIXME TODO
			playAreaCardinalDirection.SetDesiredOrientation(playAreaCardinalDirection.GetNewOrientation(rotationDirection, playAreaCardinalDirection.GetCurrentOrientation()));
			playAreaCardinalDirection.SetCurrentOrientation(playAreaCardinalDirection.GetDesiredOrientation());

			orientation.Set(playAreaCardinalDirection.GetAngleInRadiansOfOrientation(playAreaCardinalDirection.GetCurrentOrientation()));
			UpdateDirectionalWalls(registry);
		}
	}
}

void UpdateDirectionalWalls(entt::registry& registry)
{
	auto& cardinalDir = registry.get<Components::CardinalDirection>(FindEntityByTag(registry, GetTagFromContainerType(containerType_t::PLAY_AREA)));

	auto wallView = registry.view<Components::Wall, Components::DirectionallyActive, Components::Obstructs, Components::Renderable>();
	for (auto entity : wallView)
	{
		auto& wall = wallView.get<Components::Wall>(entity);
		auto& dirActive = wallView.get<Components::DirectionallyActive>(entity);
		auto& obstructs = wallView.get<Components::Obstructs>(entity);
		auto& renderable = wallView.get<Components::Renderable>(entity);

		if (dirActive.IsEnabled())
		{
			if (dirActive.IsActive(cardinalDir.GetCurrentOrientation()))
			{
				obstructs.Enable(true);
				renderable.Enable(true);
			}
			else
			{
				obstructs.Enable(false);
				renderable.Enable(false);
			}
		}
	}
}

void UpdateCensors(entt::registry& registry)
{
	auto& cardinalDir = registry.get<Components::CardinalDirection>(FindEntityByTag(registry, GetTagFromContainerType(containerType_t::PLAY_AREA)));

	auto censorView = registry.view<Components::Censor, Components::DirectionallyActive, Components::Renderable>();
	for (auto entity : censorView)
	{
		auto& censor = censorView.get<Components::Censor>(entity);
		auto& dirActive = censorView.get<Components::DirectionallyActive>(entity);
		auto& renderable = censorView.get<Components::Renderable>(entity);

		if (dirActive.IsEnabled())
		{
			if (dirActive.IsActive(cardinalDir.GetCurrentOrientation()))
			{
				censor.Enable(true);
			}
			else
			{
				censor.Enable(false);
			}
		}
		else
		{
			censor.Enable(true);
		}
	}
}

rotationDirection_t ChooseBoardRotationDirection(entt::registry& registry, const std::vector<BlockLockData>& blockLockData, const moveDirection_t& playAreaDirection, const int& linesMatched)
{
	if (linesMatched < minimumLinesMatchedToTriggerBoardRotation)
		return rotationDirection_t::NONE;

	if (blockLockData.empty())
		return rotationDirection_t::NONE;

	unsigned int blockPosition;
	unsigned int playAreaWidth;

	int desiredShift = 0;
	switch (playAreaDirection)
	{
	case moveDirection_t::NORTH:
		playAreaWidth = PlayAreaWidth;
		blockPosition = playAreaWidth / 2;

		for (BlockLockData bld : blockLockData)
		{
			if ((bld.GetCoordinates().Get().x - BufferAreaDepth) < blockPosition)
				desiredShift--;
			else
				desiredShift++;
		}
		break;
	case moveDirection_t::SOUTH:
		playAreaWidth = PlayAreaWidth;
		blockPosition = playAreaWidth / 2;

		for (BlockLockData bld : blockLockData)
		{
			if ((bld.GetCoordinates().Get().x - BufferAreaDepth) >= blockPosition)
				desiredShift--;
			else
				desiredShift++;
		}
		break;
	case moveDirection_t::EAST:
		playAreaWidth = PlayAreaHeight;
		blockPosition = playAreaWidth / 2;

		for (BlockLockData bld : blockLockData)
		{
			if ((bld.GetCoordinates().Get().y - BufferAreaDepth) < blockPosition)
				desiredShift--;
			else
				desiredShift++;
		}
		break;
	case moveDirection_t::WEST:
		playAreaWidth = PlayAreaHeight;
		blockPosition = playAreaWidth / 2;

		for (BlockLockData bld : blockLockData)
		{
			if ((bld.GetCoordinates().Get().y - BufferAreaDepth) >= blockPosition)
				desiredShift--;
			else
				desiredShift++;
		}
		break;
	default:
		break;
	}

	if (desiredShift < 0)
		return rotationDirection_t::CLOCKWISE;
	else if (desiredShift > 0)
		return rotationDirection_t::COUNTERCLOCKWISE;
	else
		return rotationDirection_t::NONE;
}

// This isn't being setup to work with plain non-Tetromino blocks, currently.
// This has severe potential issues. SPIN-27
// FIXME TODO
// DO NOT USE THIS FOR NOW
glm::uvec2 FindLowestCell(entt::registry& registry, entt::entity tetrominoEnt)
{
	if (!IsEntityTetromino(registry, tetrominoEnt))
		throw std::runtime_error("Entity is not a tetromino!");

	auto* tetromino = GetTetrominoFromEntity(registry, tetrominoEnt);
	const auto& tetCoordinate = registry.get<Components::Coordinate>(tetrominoEnt);

	auto& playAreaRefEnt = registry.get<Components::ReferenceEntity>(tetCoordinate.GetParent());
	auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaRefEnt.Get());

	entt::entity tempTetEnt = SpawnProjectedTetromino(registry, tetrominoEnt);
	auto* tempTet = GetTetrominoFromEntity(registry, tempTetEnt);
	if (!IsEntityTetromino(registry, tempTetEnt))
		throw std::runtime_error("Temporary tetromino not a tetromino!");
	auto& tempTetCoord = registry.get<Components::Coordinate>(tempTetEnt);

	entt::entity lowestCellEnt = MoveBlockInDirection(registry, tempTetEnt, playAreaDirection.GetCurrentDownDirection(), PlayAreaHeight + (BufferAreaDepth * 2)); // PlayAreaHeight is a const of 20. This is fine to be excessive with when the height is lower.
	if (lowestCellEnt == entt::null)
		throw std::runtime_error("Lowest cell entity is null!");

	const auto& lowestCell = registry.get<Components::Cell>(lowestCellEnt);
	const auto& lowestCellCoord = registry.get<Components::Coordinate>(lowestCellEnt);

	auto& tempTetMoveable = registry.get<Components::Moveable>(tempTetEnt);

	tempTetCoord.Set(lowestCellCoord.Get());
	tempTetCoord.SetParent(lowestCellCoord.GetParent());
	tempTetMoveable.SetCurrentCoordinate(lowestCellCoord);
	tempTetMoveable.SetDesiredCoordinate(tempTetMoveable.GetCurrentCoordinate());

	// Destroy the temporary Tetromino and all its blocks.
	for (int i = 0; i < 4; i++)
	{
		registry.destroy(tempTet->GetBlock(i));
	}
	registry.destroy(tempTetEnt);

	return lowestCellCoord.Get();
}

double CalculateFallSpeed(int level)
{
	return pow((0.8 - ((static_cast<double>(level) - 1) * 0.007)), static_cast<double>(level) - 1);
}