#pragma once

#include <string>
#include <entt/entity/registry.hpp>
#include "Components/Includes.h"

enum class movePiece_t
{
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	SOFT_DROP,
	HARD_DROP
};

enum class moveDirection_t
{
	NORTH,
	SOUTH,
	EAST,
	WEST
};

// Ensure GetTagFromContainerType() and this enum are in sync
enum class containerType_t
{
	PLAY_AREA = 0,
	MATRIX,
	BUFFER,
	BAG_AREA
};
// Ensure containerType_t and this function are in sync
const std::string GetTagFromContainerType(const containerType_t& t);

entt::entity FindContainerEntityByTag(entt::registry& registry, const std::string& tagName);
const std::string FindTagOfContainerEntity(entt::registry& registry, const entt::entity& containerEntity);
bool CanOccupyCell(entt::registry& registry, const std::string& containerTag, const entt::entity& cellEntity, const bool& disableObstruction = false);
const Components::Coordinate& GetCoordinateOfEntity(entt::registry& registry, const entt::entity& entity);
const Components::Cell& GetCellOfEntity(entt::registry& registry, const entt::entity& entity);
Components::Cell& GetCellAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
entt::entity GetCellAtCoordinates2(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
const Components::Block& GetBlockAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
entt::entity MoveBlockInDirection(entt::registry& registry, const std::string& containerTag, const entt::entity& blockEnt, const moveDirection_t& direction, const unsigned int& distance, const bool& disableObstruction = false);
entt::entity GetCellLinkAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate, const moveDirection_t& direction);
entt::entity GetActiveControllable(entt::registry& registry);