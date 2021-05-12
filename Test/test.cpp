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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(1, 0));
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(0, 0)); // Move was not allowed, same spot.
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(2, 0)); // Move was allowed, moved 2 east.
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(2, 0)); // Move did NOT go 3, only went 2 because it was obstructed.
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
		EXPECT_TRUE(beginCoord.GetParent() == endCoord.GetParent() && endCoord.Get() == glm::uvec2(0, 0)); // Move did NOT go 1, only went 0 because it was obstructed.
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 3), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 3), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 3), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(4, 4), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(4, 4), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(3, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto buffer = registry.create();
	registry.emplace<Components::Position>(buffer, glm::vec3(displayData.x / 2, displayData.y / 2, 0.0f));
	registry.emplace<Components::Scale>(buffer);
	registry.emplace<Components::Container2>(buffer, glm::uvec2(3, 2), glm::vec2(25, 25));
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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(testPlayAreaWidth + (BufferAreaDepth * 2), testPlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);



	BuildGrid(registry, matrix);

	SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
		Components::Coordinate(FindContainerEntityByTag(registry,
		GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 6)),
		tetrominoType_t::I);

	// Where are blocks?
	int numBlocksInExpectedCoordinates = 0;
	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);
		auto& coordinate = blockView.get<Components::Coordinate>(entity);
		Components::Coordinate beginCoord = GetCoordinateOfEntity(registry, entity);

		if (beginCoord.Get() == glm::uvec2(3, 6))
			numBlocksInExpectedCoordinates++;
		else if (beginCoord.Get() == glm::uvec2(4, 6))
			numBlocksInExpectedCoordinates++;
		else if (beginCoord.Get() == glm::uvec2(5, 6))
			numBlocksInExpectedCoordinates++;
		else if (beginCoord.Get() == glm::uvec2(6, 6))
			numBlocksInExpectedCoordinates++;
		else
			EXPECT_TRUE(false); // Error here immediately.		
	}
	EXPECT_TRUE(numBlocksInExpectedCoordinates == 4);

	MovePiece(registry, movePiece_t::MOVE_RIGHT);

	double fakeCurrentFrameTime = 10000; // Arbitrarily large number, so any timers are exceeded.
	Systems::MovementSystem(registry, fakeCurrentFrameTime);

	// Where are blocks?

}