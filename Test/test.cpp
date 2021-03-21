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