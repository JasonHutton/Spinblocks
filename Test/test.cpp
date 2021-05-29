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

#include "Systems/GenerationSystem.h"
#include "Systems/FallingSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/StateChangeSystem.h"
#include "Systems/PatternSystem.h"
#include "Systems/EliminateSystem.h"
#include "Systems/BoardRotateSystem.h"
#include "Systems/DetachSystem.h"
#include "Systems/CompletionSystem.h"
#include "Systems/SoundSystem.h"

#include "Input/InputHandler.h"
#include "Input/GameInput.h"
#include "AudioManager.h"

#include "GameState.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

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
	registry.emplace<Components::Container>(playArea, glm::uvec2(1, 1), glm::vec2(25, 25));
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
	registry.emplace<Components::Container>(playArea, glm::uvec2(2, 1), glm::vec2(25, 25));
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
	registry.emplace<Components::Container>(playArea, glm::uvec2(1, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container>(playArea, glm::uvec2(2, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 1), glm::vec2(25, 25));
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
	registry.emplace<Components::Container>(playArea, glm::uvec2(1, 3), glm::vec2(25, 25));
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
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
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
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(1, 0));
	}
}

TEST(ObstructionTest, Step1SouthObstructedByNull) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(0, 0)); // Move was not allowed, same spot.
	}
}

TEST(ObstructionTest, Step2EastClear) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(2, 0)); // Move was allowed, moved 2 east.
	}
}

TEST(ObstructionTest, Step3EastObstructedByNull) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(2, 0)); // Move did NOT go 3, only went 2 because it was obstructed.
	}
}

TEST(ObstructionTest, Step1NorthObstructedByBlock) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(0, 0)); // Move did NOT go 1, only went 0 because it was obstructed.
	}
}

TEST(ObstructionTest, Step2NorthObstructedByBlock) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container>(playArea, glm::uvec2(3, 3), glm::vec2(25, 25));
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(0, 1)); // Move did NOT go 2, only went 1 because it was obstructed.
	}
}

TEST(PatternTest, Pattern0LinesFound) {
	entt::registry registry;

	const auto playArea = registry.create();
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);


	BuildGrid(registry, matrix);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);

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
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);


	BuildGrid(registry, matrix);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 0)), false);

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
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 3), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);

	BuildGrid(registry, matrix);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 1)), false);

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
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(4, 4), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);


	BuildGrid(registry, matrix);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 2)), false);

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
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(4, 4), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);


	BuildGrid(registry, matrix);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 3)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	int linesFound = Systems::PatternSystem(registry, 4, 0);

	EXPECT_TRUE(linesFound == 4);
}

TEST(CellLinkTest, Step1SouthFromOutClear) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));
	
	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);
	
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 1)));

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 1));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 1);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), buffer);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 0));
	}
}

TEST(CellLinkTest, Step1SouthFromInClear) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 0)));

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 1);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), matrix);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 1));
	}
}

TEST(CellLinkTest, Step2SouthFromOutClear) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 1)));

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 1));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), matrix);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 1));
	}
}

TEST(CellLinkTest, Step2SouthFromInClear) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 0)));

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), matrix);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 0));
	}
}

TEST(CellLinkTest, Step1SouthFromOutObstructed) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 1)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 0)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 1));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 1);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), buffer);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 1));
	}
}

TEST(CellLinkTest, Step1SouthFromInObstructed) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 0)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 1);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), buffer);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 0));
	}
}

TEST(CellLinkTest, Step2SouthFromOutObstructed1) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 1)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 0)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 1));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), buffer);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 1));
	}
}

TEST(CellLinkTest, Step2SouthFromOutObstructed2) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 1)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 1));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), buffer);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 0));
	}
}

TEST(CellLinkTest, Step2SouthFromInObstructed1) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 0)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), buffer);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 0));
	}
}

TEST(CellLinkTest, Step2SouthFromInObstructed2) {
	entt::registry registry;

	const auto matrix = registry.create();
	registry.emplace<Components::Position>(matrix, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(buffer, GetTagFromContainerType(containerType_t::BUFFER));

	BuildGrid(registry, matrix);
	BuildGrid(registry, buffer);

	LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(1, 1)), Components::Coordinate(buffer, glm::uvec2(1, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	LinkCoordinates(registry, Components::Coordinate(buffer, glm::uvec2(1, 0)), Components::Coordinate(matrix, glm::uvec2(1, 1)), moveDirection_t::SOUTH, moveDirection_t::SOUTH);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::BUFFER), Components::Coordinate(buffer, glm::uvec2(1, 0)));
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);

	auto blockView = registry.view<Components::Block, Components::Coordinate, Components::Controllable>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);

		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);
		EXPECT_EQ(beginCoord.GetParent(), buffer);
		EXPECT_TRUE(beginCoord.Get() == glm::uvec2(1, 0));
		entt::entity endCellEnt = MoveBlockInDirection(registry, entity, moveDirection_t::SOUTH, 2);
		Components::Coordinate endCoord = GetCoordinateOfEntity(registry, endCellEnt);
		EXPECT_EQ(endCoord.GetParent(), matrix);
		EXPECT_TRUE(endCoord.Get() == glm::uvec2(1, 1));
	}
}

bool ValidateBlockPositions(entt::registry& registry, const glm::uvec2& block1, const glm::uvec2& block2, const glm::uvec2& block3, const glm::uvec2& block4)
{
	int numBlocksInExpectedCoordinates = 0;

	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);
		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);

		if (numBlocksInExpectedCoordinates >= 4)
			continue;

		if (beginCoord.Get() == block1)
			numBlocksInExpectedCoordinates++;
		else if (beginCoord.Get() == block2)
			numBlocksInExpectedCoordinates++;
		else if (beginCoord.Get() == block3)
			numBlocksInExpectedCoordinates++;
		else if (beginCoord.Get() == block4)
			numBlocksInExpectedCoordinates++;
	}

	if (numBlocksInExpectedCoordinates == 4)
		return true;

	return false;
}

TEST(TetrominoMovementTest, Step1EastClear) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
		GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));
	
	MovePiece(registry, movePiece_t::MOVE_RIGHT);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	//Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6), glm::uvec2(7, 6)));
}

TEST(TetrominoMovementTest, Step1SouthClear) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));

	MovePiece(registry, movePiece_t::SOFT_DROP);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	//Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 5), glm::uvec2(4, 5), glm::uvec2(5, 5), glm::uvec2(6, 5)));
}

TEST(TetrominoMovementTest, Step1WestClear) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));

	MovePiece(registry, movePiece_t::MOVE_LEFT);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	//Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(2, 6), glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6)));
}

TEST(TetrominoMovementObstructionTest, Step1EastObstructedByNull) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);
	// 5 + 6 + 5 = 16 - 3 = 13
	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(14, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));

	MovePiece(registry, movePiece_t::MOVE_RIGHT);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	//Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));
}

TEST(TetrominoMovementObstructionTest, Step1EastObstructedByWall) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	for (int i = BufferAreaDepth - 1; i < testPlayAreaHeight + BufferAreaDepth + 1; i++)
	{
		//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Border 3", Components::Coordinate(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth - 1) + 1, i)));
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth - 1) + 1, i)), true, { moveDirection_t::NORTH, moveDirection_t::SOUTH, moveDirection_t::EAST });
	}

	// 5 + 6 = 11 - 3 = 8
	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(9, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));

	MovePiece(registry, movePiece_t::MOVE_RIGHT);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	//Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));
}

TEST(TetrominoRotationTest, Rotate1ClockwiseClear) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	//Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(5, 4), glm::uvec2(5, 5), glm::uvec2(5, 6), glm::uvec2(5, 7)));
}

TEST(TetrominoRotationTest, Rotate2ClockwiseClear) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	double fakeCurrentFrameTime;
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 5), glm::uvec2(4, 5), glm::uvec2(5, 5), glm::uvec2(6, 5)));
}

TEST(TetrominoRotationTest, Rotate3ClockwiseClear) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	double fakeCurrentFrameTime;
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 30000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(4, 4), glm::uvec2(4, 5), glm::uvec2(4, 6), glm::uvec2(4, 7)));
}

TEST(TetrominoRotationTest, Rotate4ClockwiseClear) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	double fakeCurrentFrameTime;
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 30000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 40000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));
}

TEST(TetrominoRotationObstructionTest, Rotate1ClockwiseObstructedByNull) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	// 5 + 6 + 5 = 16 - 3 = 13
	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(14, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));

	double fakeCurrentFrameTime;

	MovePiece(registry, movePiece_t::MOVE_RIGHT);
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// Move failed, still in the same spot. Was obstructed, so is right against the obstruction.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));
	// Now try rotating.

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// We should have rotated, and so no longer be in the same position
	EXPECT_FALSE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));
	// And we should end up here.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(15, 4), glm::uvec2(15, 5), glm::uvec2(15, 6), glm::uvec2(15, 7)));
}

TEST(TetrominoRotationObstructionTest, Rotate1CounterClockwiseObstructedByNull) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	// 5 + 6 + 5 = 16 - 3 = 13
	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(14, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));

	double fakeCurrentFrameTime;

	MovePiece(registry, movePiece_t::MOVE_RIGHT);
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// Move failed, still in the same spot. Was obstructed, so is right against the obstruction.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));
	// Now try rotating.

	RotatePiece(registry, rotatePiece_t::ROTATE_COUNTERCLOCKWISE);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// We should have rotated, and so no longer be in the same position
	EXPECT_FALSE(ValidateBlockPositions(registry, glm::uvec2(13, 6), glm::uvec2(14, 6), glm::uvec2(15, 6), glm::uvec2(16, 6)));
	// And we should end up here.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(15, 4), glm::uvec2(15, 5), glm::uvec2(15, 6), glm::uvec2(15, 7)));
}

TEST(TetrominoRotationObstructionTest, Rotate1ClockwiseObstructedByWall) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	for (int i = BufferAreaDepth - 1; i < testPlayAreaHeight + BufferAreaDepth + 1; i++)
	{
		//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Border 3", Components::Coordinate(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth - 1) + 1, i)));
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth - 1) + 1, i)), true, { moveDirection_t::NORTH, moveDirection_t::SOUTH, moveDirection_t::EAST });
	}

	// 5 + 6 = 11 - 3 = 8
	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(9, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));

	double fakeCurrentFrameTime;

	MovePiece(registry, movePiece_t::MOVE_RIGHT);
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// Move failed, still in the same spot. Was obstructed, so is right against the obstruction.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));
	// Now try rotating.

	RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// We should have rotated, and so no longer be in the same position
	EXPECT_FALSE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));
	// And we should end up here.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(10, 4), glm::uvec2(10, 5), glm::uvec2(10, 6), glm::uvec2(10, 7)));
}

TEST(TetrominoRotationObstructionTest, Rotate1CounterClockwiseObstructedByWall) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	for (int i = BufferAreaDepth - 1; i < testPlayAreaHeight + BufferAreaDepth + 1; i++)
	{
		//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Border 3", Components::Coordinate(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth - 1) + 1, i)));
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth - 1) + 1, i)), true, { moveDirection_t::NORTH, moveDirection_t::SOUTH, moveDirection_t::EAST });
	}

	// 5 + 6 = 11 - 3 = 8
	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(9, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));

	double fakeCurrentFrameTime;

	MovePiece(registry, movePiece_t::MOVE_RIGHT);
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// Move failed, still in the same spot. Was obstructed, so is right against the obstruction.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));
	// Now try rotating.

	RotatePiece(registry, rotatePiece_t::ROTATE_COUNTERCLOCKWISE);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// We should have rotated, and so no longer be in the same position
	EXPECT_FALSE(ValidateBlockPositions(registry, glm::uvec2(8, 6), glm::uvec2(9, 6), glm::uvec2(10, 6), glm::uvec2(11, 6)));
	// And we should end up here.
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(9, 4), glm::uvec2(9, 5), glm::uvec2(9, 6), glm::uvec2(9, 7)));
}

TEST(TetrominoMovementObstructionTest, Step1SouthObstructedByBlock) {
	entt::registry registry;

	int testPlayAreaWidth = 6;
	int testPlayAreaHeight = 6;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(4, 5)), false);

	auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
			GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Needed for MovementSystem()
	auto* tetromino = GetTetrominoFromEntity(registry, tet);
	if (registry.all_of<Components::Moveable>(tet))
	{
		auto& moveable = registry.get<Components::Moveable>(tet);
		moveable.SetMovementState(Components::movementStates_t::FALL);

		for (int i = 0; i < 4; i++)
		{
			auto& blockMoveable = registry.get<Components::Moveable>(tetromino->GetBlock(i));
			if (registry.all_of<Components::Follower>(tetromino->GetBlock(i)))
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FOLLOWING);
			}
			else
			{
				blockMoveable.SetMovementState(Components::movementStates_t::FALL);
			}
		}
	}

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));

	MovePiece(registry, movePiece_t::SOFT_DROP);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	//Systems::MovementSystem(registry, fakeCurrentFrameTime);

	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(3, 6), glm::uvec2(4, 6), glm::uvec2(5, 6), glm::uvec2(6, 6)));
}

TEST(CollapseTest, Collapse1Test) {
	entt::registry registry;

	int testPlayAreaWidth = 3;
	int testPlayAreaHeight = 3;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	// These are going to be eliminated
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 0)), false);

	// These are going to fall
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 2)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	double fakeCurrentFrameTime;
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	int linesFound = Systems::PatternSystem(registry, testPlayAreaWidth, fakeCurrentFrameTime);

	EXPECT_TRUE(linesFound == 1);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::EliminateSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 30000; // Arbitrarily large number, so any timers are exceeded.

	// Ensure the hovering blocks fall.
	Systems::FallingSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 40000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	auto blockLockData = std::vector<BlockLockData>();
	fakeCurrentFrameTime = 50000; // Arbitrarily large number, so any timers are exceeded.
	Systems::StateChangeSystem(registry, fakeCurrentFrameTime, blockLockData);
	fakeCurrentFrameTime = 60000; // Arbitrarily large number, so any timers are exceeded.
	linesFound = Systems::PatternSystem(registry, testPlayAreaWidth, fakeCurrentFrameTime);

	// Is the row gone?
	EXPECT_TRUE(linesFound == 0);

	// Are the remaining blocks still there, and have they fallen?
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(0, 0), glm::uvec2(1, 0), glm::uvec2(0, 1), glm::uvec2(1, 1)));
}

TEST(CollapseTest, Collapse2Test) {
	entt::registry registry;

	int testPlayAreaWidth = 3;
	int testPlayAreaHeight = 4;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	// These are going to be eliminated
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 1)), false);

	// These are going to fall
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 3)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	double fakeCurrentFrameTime;
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	int linesFound = Systems::PatternSystem(registry, testPlayAreaWidth, fakeCurrentFrameTime);

	EXPECT_TRUE(linesFound == 2);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::EliminateSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 30000; // Arbitrarily large number, so any timers are exceeded.

	// Ensure the hovering blocks fall.
	Systems::FallingSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 40000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	auto blockLockData = std::vector<BlockLockData>();
	fakeCurrentFrameTime = 50000; // Arbitrarily large number, so any timers are exceeded.
	Systems::StateChangeSystem(registry, fakeCurrentFrameTime, blockLockData);
	fakeCurrentFrameTime = 60000; // Arbitrarily large number, so any timers are exceeded.
	linesFound = Systems::PatternSystem(registry, testPlayAreaWidth, fakeCurrentFrameTime);

	// Is the row gone?
	EXPECT_TRUE(linesFound == 0);

	// Are the remaining blocks still there, and have they fallen?
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(0, 0), glm::uvec2(1, 0), glm::uvec2(0, 1), glm::uvec2(1, 1)));
}

TEST(CollapseTest, Collapse1GapTest) {
	entt::registry registry;

	int testPlayAreaWidth = 4;
	int testPlayAreaHeight = 5;

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * testPlayAreaWidth, cellHeight * testPlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (testPlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (testPlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	// These are going to be eliminated
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 0)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2, 2)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 2)), false);

	// These are just in between the eliminated lines, and will fall
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 1)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(3, 1)), false);

	// These are also going to fall, but will be obstructed by the above in between blocks.
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 3)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0, 4)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1, 4)), false);

	auto blockView = registry.view<Components::Block, Components::Moveable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
	}

	double fakeCurrentFrameTime;
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	int linesFound = Systems::PatternSystem(registry, testPlayAreaWidth, fakeCurrentFrameTime);

	EXPECT_TRUE(linesFound == 2);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::EliminateSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 30000; // Arbitrarily large number, so any timers are exceeded.

	// Ensure the hovering blocks fall.
	Systems::FallingSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 40000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	auto blockLockData = std::vector<BlockLockData>();
	fakeCurrentFrameTime = 50000; // Arbitrarily large number, so any timers are exceeded.
	Systems::StateChangeSystem(registry, fakeCurrentFrameTime, blockLockData);
	fakeCurrentFrameTime = 60000; // Arbitrarily large number, so any timers are exceeded.
	linesFound = Systems::PatternSystem(registry, testPlayAreaWidth, fakeCurrentFrameTime);

	// Is the row gone?
	EXPECT_TRUE(linesFound == 0);

	// Are the remaining blocks still there, and have they fallen?
	EXPECT_TRUE(ValidateBlockPositions(registry, glm::uvec2(0, 1), glm::uvec2(1, 1), glm::uvec2(0, 2), glm::uvec2(1, 2)));
}

TEST(PlayAreaRotationTest, BlockPositionsTest) {
	entt::registry registry;

	// Some of the things we're testing aren't written to be simplfied, so we need to use a full-size board with buffers and walls, or it won't work properly.

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * PlayAreaWidth, cellHeight * PlayAreaHeight));
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));
	registry.emplace<Components::Rotateable>(playArea, 0.0f, 0.0f);
	registry.emplace<Components::Orientation>(playArea, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	registry.emplace<Components::InheritScalingFromParent>(playArea, false);
	registry.emplace<Components::CardinalDirection>(playArea);

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	//registry.emplace<Components::Scale>(matrix, glm::uvec2(1, 1));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * (PlayAreaWidth + (BufferAreaDepth * 2)), cellHeight * (PlayAreaHeight + (BufferAreaDepth * 2))));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::Container>(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth * 2), PlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);

	BuildGrid(registry, matrix);

	for (int i = BufferAreaDepth - 1; i < PlayAreaWidth + BufferAreaDepth + 1; i++)
	{
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(i, PlayAreaHeight + (BufferAreaDepth - 1) + 1)), true, { moveDirection_t::SOUTH, moveDirection_t::EAST, moveDirection_t::WEST });
	}
	for (int i = BufferAreaDepth - 1; i < PlayAreaWidth + BufferAreaDepth + 1; i++)
	{
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(i, 0 + (BufferAreaDepth - 1))), true, { moveDirection_t::NORTH, moveDirection_t::EAST, moveDirection_t::WEST });
	}
	for (int i = BufferAreaDepth - 1; i < PlayAreaHeight + BufferAreaDepth + 1; i++)
	{
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth - 1) + 1, i)), true, { moveDirection_t::NORTH, moveDirection_t::SOUTH, moveDirection_t::EAST });
	}
	for (int i = BufferAreaDepth - 1; i < PlayAreaHeight + BufferAreaDepth + 1; i++)
	{
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(0 + (BufferAreaDepth - 1), i)), true, { moveDirection_t::NORTH, moveDirection_t::SOUTH, moveDirection_t::WEST });
	}

	UpdateDirectionalWalls(registry);

	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0 + BufferAreaDepth, 0 + BufferAreaDepth)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1 + BufferAreaDepth, 0 + BufferAreaDepth)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(1 + BufferAreaDepth, 1 + BufferAreaDepth)), false);
	SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(2 + BufferAreaDepth, 1 + BufferAreaDepth)), false);
	
	auto blockView = registry.view<Components::Block, Components::Moveable, Components::Obstructable>();
	for (auto entity : blockView)
	{
		auto& moveable = blockView.get<Components::Moveable>(entity);
		auto& obstructable = blockView.get<Components::Obstructable>(entity);
		moveable.SetMovementState(Components::movementStates_t::LOCKED);
		obstructable.SetIsObstructed(true); // We need this in testing because this gets set during falling, which we're skipping hwne placing on the fly.
	}

	// Z Tetromino arrangement of blocks.
	EXPECT_TRUE(ValidateBlockPositions(registry, 
		glm::uvec2(0 + BufferAreaDepth, 0 + BufferAreaDepth), 
		glm::uvec2(1 + BufferAreaDepth, 0 + BufferAreaDepth), 
		glm::uvec2(1 + BufferAreaDepth, 1 + BufferAreaDepth), 
		glm::uvec2(2 + BufferAreaDepth, 1 + BufferAreaDepth)));

	auto& playAreaDirection = registry.get<Components::CardinalDirection>(playArea);
	EXPECT_TRUE(playAreaDirection.GetCurrentOrientation() == moveDirection_t::NORTH);

	double fakeCurrentFrameTime;
	fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::BoardRotateSystem(registry, fakeCurrentFrameTime, rotationDirection_t::CLOCKWISE);

	EXPECT_TRUE(playAreaDirection.GetCurrentOrientation() == moveDirection_t::EAST);

	fakeCurrentFrameTime = 20000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 30000; // Arbitrarily large number, so any timers are exceeded.
	auto blockLockData = std::vector<BlockLockData>();
	Systems::StateChangeSystem(registry, fakeCurrentFrameTime, blockLockData);
	// Now detach, after the falling realignment.
	fakeCurrentFrameTime = 40000; // Arbitrarily large number, so any timers are exceeded.
	Systems::DetachSystem(registry, fakeCurrentFrameTime);

	// Process next iteraton of loop
	fakeCurrentFrameTime = 50000; // Arbitrarily large number, so any timers are exceeded.
	Systems::FallingSystem(registry, fakeCurrentFrameTime);
	fakeCurrentFrameTime = 60000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);
	blockLockData.clear();
	fakeCurrentFrameTime = 70000; // Arbitrarily large number, so any timers are exceeded.
	Systems::StateChangeSystem(registry, fakeCurrentFrameTime, blockLockData);

	// Z Tetromino has collapsed into a simple square, against the lower-right side of the coordinate matrix
	EXPECT_TRUE(ValidateBlockPositions(registry, 
		glm::uvec2(8 + BufferAreaDepth, 0 + BufferAreaDepth),
		glm::uvec2(9 + BufferAreaDepth, 0 + BufferAreaDepth),
		glm::uvec2(8 + BufferAreaDepth, 1 + BufferAreaDepth),
		glm::uvec2(9 + BufferAreaDepth, 1 + BufferAreaDepth)));
}