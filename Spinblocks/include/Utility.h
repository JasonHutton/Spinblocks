#pragma once

#include <string>
#include <entt/entity/registry.hpp>
#include "Components/Includes.h"

entt::entity FindContainerEntityByTag(entt::registry& registry, const std::string& tagName);
const std::string FindTagOfContainerEntity(entt::registry& registry, const entt::entity& containerEntity);
bool CanOccupyCell(entt::registry& registry, const std::string& containerTag, const entt::entity& cellEntity);
const Components::Coordinate& GetCoordinateOfEntity(entt::registry& registry, const entt::entity& entity);
const Components::Cell& GetCellOfEntity(entt::registry& registry, const entt::entity& entity);
Components::Cell& GetCellAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);
const Components::Block& GetBlockAtCoordinates(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& coordinate);