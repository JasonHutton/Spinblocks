#include "pch.h"

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE // Just disables the default OpenGL environment explicitly. GLAD should be detected anyway.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <entt/entity/registry.hpp>

// Include GLM core features
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/shader.h>
#include <learnopengl/mesh.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

#include <string>
#include <iostream>
#include <vector>

#include "Systems/SystemShared.h"

#include "Components/Includes.h"
#include "GameTime.h"
#include "Globals.h"
#include "Utility.h"

#include "Systems/FallingSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/StateChangeSystem.h"
#include "Systems/PatternSystem.h"
#include "Systems/EliminateSystem.h"

#include "Input/InputHandler.h"
#include "Input/GameInput.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;

/*TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}*/

TEST(GridTest, Grid1x1) {
	entt::registry registry;
	
	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(1, 1), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));
	
	BuildGrid(registry, playArea);

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);

		if (coordinate.Get().x == 0 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
	}
}

TEST(GridTest, Grid2x1) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(2, 1), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);

		if (coordinate.Get().x == 0 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 1 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
	}
}

TEST(GridTest, Grid1x2) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(1, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);

		if (coordinate.Get().x == 0 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 0 && coordinate.Get().y == 1)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
	}
}

TEST(GridTest, Grid2x2) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(2, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);

		if (coordinate.Get().x == 0 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 0 && coordinate.Get().y == 1)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 1 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
		if (coordinate.Get().x == 1 && coordinate.Get().y == 1)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
	}
}

TEST(GridTest, Grid3x1) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 1), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);

		if (coordinate.Get().x == 0 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 1 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
		if (coordinate.Get().x == 2 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
	}
}

TEST(GridTest, Grid1x3) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(1, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);

		if (coordinate.Get().x == 0 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 0 && coordinate.Get().y == 1)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 0 && coordinate.Get().y == 2)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
	}
}

TEST(GridTest, Grid3x3) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);

		if (coordinate.Get().x == 0 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 1 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
		if (coordinate.Get().x == 2 && coordinate.Get().y == 0)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() == entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
		if (coordinate.Get().x == 0 && coordinate.Get().y == 1)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 1 && coordinate.Get().y == 1)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
		if (coordinate.Get().x == 2 && coordinate.Get().y == 1)
		{
			EXPECT_TRUE(cell.GetNorth() != entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
		if (coordinate.Get().x == 0 && coordinate.Get().y == 2)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() == entt::null);
		}
		if (coordinate.Get().x == 1 && coordinate.Get().y == 2)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() != entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
		if (coordinate.Get().x == 2 && coordinate.Get().y == 2)
		{
			EXPECT_TRUE(cell.GetNorth() == entt::null);
			EXPECT_TRUE(cell.GetEast() == entt::null);
			EXPECT_TRUE(cell.GetSouth() != entt::null);
			EXPECT_TRUE(cell.GetWest() != entt::null);
		}
	}
}

TEST(ObstructionTest, Step1EastClear) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);
	
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)));
	
	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);
		
		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(0, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::EAST, 1);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && beginCoord.Get().y == endCoord.Get().y && beginCoord.Get().x + 1 == endCoord.Get().x);
	}
}

TEST(ObstructionTest, Step1SouthObstructedByNull) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)));

	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(0, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 1);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && beginCoord.Get() == endCoord.Get()); // Move was not allowed, same spot.
	}
}

TEST(ObstructionTest, Step2EastClear) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)));

	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(0, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::EAST, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && beginCoord.Get().y == endCoord.Get().y && beginCoord.Get().x + 2 == endCoord.Get().x);
	}
}

TEST(ObstructionTest, Step3EastObstructedByNull) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)));

	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(0, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::EAST, 3);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && beginCoord.Get().y == endCoord.Get().y && beginCoord.Get().x + 2 == endCoord.Get().x); // Move did NOT go 3, only went 2 because it was obstructed.
	}
}

TEST(ObstructionTest, Step1NorthObstructedByBlock) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 1)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(0, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::NORTH, 1);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && beginCoord.Get().y == endCoord.Get().y && beginCoord.Get().x == endCoord.Get().x);
	}
}

TEST(ObstructionTest, Step2NorthObstructedByBlock) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 2)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(0, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::NORTH, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && beginCoord.Get().y == endCoord.Get().y && beginCoord.Get().x == endCoord.Get().x);
	}
}

TEST(PatternTest, Pattern0LinesFound) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 0)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	int linesFound = Systems::PatternSystem(registry, 3, 0);

	EXPECT_TRUE(linesFound == 0);
}

TEST(PatternTest, Pattern1LineFound) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 0)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	int linesFound = Systems::PatternSystem(registry, 3, 0);

	EXPECT_TRUE(linesFound == 1);
}

TEST(PatternTest, Pattern2LinesFound) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 1)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	int linesFound = Systems::PatternSystem(registry, 3, 0);

	EXPECT_TRUE(linesFound == 2);
}

TEST(PatternTest, Pattern3LinesFound) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(4, 4), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(3, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(3, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(3, 2)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	int linesFound = Systems::PatternSystem(registry, 4, 0);

	EXPECT_TRUE(linesFound == 3);
}

TEST(PatternTest, Pattern4LinesFound) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(4, 4), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::MATRIX));


	BuildGrid(registry, playArea);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(3, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(3, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(3, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(0, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(1, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(2, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(playArea, glm::uvec2(3, 3)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	int linesFound = Systems::PatternSystem(registry, 4, 0);

	EXPECT_TRUE(linesFound == 4);
}