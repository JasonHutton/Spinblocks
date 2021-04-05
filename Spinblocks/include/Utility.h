#pragma once

#include "Components/Tetrominos/Tetromino.h" // Not sure why this is required, since it's in Components/Includes.h, but there are issues with Tetromino without it...

#include <entt/entity/registry.hpp>
#include "Components/Includes.h"
#include "Systems/SystemShared.h"

#include <string>

// Ensure containerType_t and this function are in sync
const std::string GetTagFromContainerType(const containerType_t& t);

entt::entity FindContainerEntityByTag(entt::registry& registry, const std::string& tagName);
const std::string FindTagOfContainerEntity(entt::registry& registry, const entt::entity& containerEntity);
bool CanOccupyCell(entt::registry& registry, const entt::entity& blockEnt, const entt::entity& cellEntity, const bool& disableObstruction = false);
const Components::Coordinate& GetCoordinateOfEntity(entt::registry& registry, const entt::entity& entity);
const Components::Cell& GetCellOfEntity(entt::registry& registry, const entt::entity& entity);
Components::Cell& GetCellAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
entt::entity GetCellAtCoordinates2(entt::registry& registry, const Components::Coordinate& coordinate);
const Components::Block& GetBlockAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
entt::entity MoveBlockInDirection(entt::registry& registry, const entt::entity& blockEnt, const moveDirection_t& direction, const unsigned int& distance, const bool& disableObstruction = false);
entt::entity GetCellLinkAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate, const moveDirection_t& direction);
entt::entity GetActiveControllable(entt::registry& registry);
void BuildGrid(entt::registry& registry, const entt::entity& parentEntity);
entt::entity SpawnBlock(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, const bool& isControllable = true);
void LinkCoordinates(entt::registry& registry, const Components::Coordinate& origin, const Components::Coordinate& destination, const moveDirection_t& moveDir, const moveDirection_t& moveDirReverse);
entt::entity SpawnTetromino(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& spawnCoordinate, const tetrominoType_t& tetrominoType, const bool& isControllable = true);
bool IsEntityTetromino(entt::registry& registry, entt::entity ent);
Components::Tetromino* GetTetrominoFromEntity(entt::registry& registry, entt::entity entity);
bool IsAnyBlockInTetrominoObstructingSelf(entt::registry& registry, entt::entity entity, const entt::entity& cellEntity, const bool& disableObstruction);