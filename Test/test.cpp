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

#include "Components/Includes.h"
#include "GameTime.h"
#include "Globals.h"

void BuildGrid(entt::registry& registry, const entt::entity& parentEntity)
{
	Components::Container2 container2 = registry.get<Components::Container2>(parentEntity);
	// We want a copy of this to be stored in this scope, because the component reference may change without warning.
	// We could also just store the vector coordinate. Either way.
	Components::Position parentPosition = registry.get<Components::Position>(parentEntity);

	for (int i = 0; i < container2.GetGridDimensions().x; i++)
	{
		for (int k = 0; k < container2.GetGridDimensions().y; k++)
		{
			std::string tagName = "Grid";
			tagName += std::to_string(i);
			tagName += "-";
			tagName += std::to_string(k);

			const auto cell = registry.create();
			registry.emplace<Components::Coordinate>(cell, glm::uvec2(i, k));
			registry.emplace<Components::Cell>(cell, parentEntity);
			registry.emplace<Components::Tag>(cell, tagName);
			registry.emplace<Components::Scale>(cell, container2.GetCellDimensions3());
			registry.emplace<Components::Position>(cell, container2.GetCellPosition3(parentPosition.Get(), glm::uvec2(i, k)));
			//registry.emplace<Components::Renderable>(cell, Model("./data/block/grey.obj"));
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
					cell1.SetSouth(entity2);
				}

				if (coordinate1.Get().y - 1 == coordinate2.Get().y)
				{
					cell1.SetNorth(entity2);
				}
			}
		}
	}
}

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
	registry.emplace<Components::Tag>(playArea, "Play Area");

	
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
	registry.emplace<Components::Tag>(playArea, "Play Area");


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