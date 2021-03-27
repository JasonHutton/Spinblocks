#pragma once

#include <string>
#include <entt/entity/registry.hpp>
#include "Components/Includes.h"
#include "Systems/SystemShared.h"

// Ensure containerType_t and this function are in sync
const std::string GetTagFromContainerType(const containerType_t& t);

entt::entity FindContainerEntityByTag(entt::registry& registry, const std::string& tagName);
const std::string FindTagOfContainerEntity(entt::registry& registry, const entt::entity& containerEntity);
bool CanOccupyCell(entt::registry& registry, const entt::entity& cellEntity, const bool& disableObstruction = false);
const Components::Coordinate& GetCoordinateOfEntity(entt::registry& registry, const entt::entity& entity);
const Components::Cell& GetCellOfEntity(entt::registry& registry, const entt::entity& entity);
Components::Cell& GetCellAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
entt::entity GetCellAtCoordinates2(entt::registry& registry, const Components::Coordinate& coordinate);
const Components::Block& GetBlockAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
entt::entity MoveBlockInDirection(entt::registry& registry, const entt::entity& blockEnt, const moveDirection_t& direction, const unsigned int& distance, const bool& disableObstruction = false);
entt::entity GetCellLinkAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate, const moveDirection_t& direction);
entt::entity GetActiveControllable(entt::registry& registry);
void BuildGrid(entt::registry& registry, const entt::entity& parentEntity);
void SpawnBlock(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, const bool& isControllable = true);
void LinkCoordinates(entt::registry& registry, const Components::Coordinate& origin, const Components::Coordinate& destination, const moveDirection_t& moveDir, const moveDirection_t& moveDirReverse);
void SpawnTetromino(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, const tetrominoType_t& tetrominoType, const bool& isControllable = true);