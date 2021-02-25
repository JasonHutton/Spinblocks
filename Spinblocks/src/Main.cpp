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

using std::string;
using std::cout;
using std::endl;
using std::vector;

template<class T>
T* Coalesce(T* value, T* defaultValue)
{
	return value != NULL ? value : defaultValue;
}

void glfwErrorCallback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
auto shaders = std::unordered_map<std::string, Shader*>();

Shader* RetrieveShader(const char* key, const char* vs, const char* fs)
{
	if (shaders.count(key) > 0)
	{
		return shaders[key];
	}
	else
	{
		return (shaders[key] = new Shader(vs, fs));
	}
}

//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void update(entt::registry& registry) {
	// Views get created when queried. It exposes internal data structures of the registry to itself.
	// Views are cheap to make/destroy.
	// Views are meant to be temporary; don't store them after

	auto gameObjectView = registry.view<Components::GameObject, Components::Position>();
	for (auto entity : gameObjectView)
	{
		auto& gameObject = gameObjectView.get<Components::GameObject>(entity);
		if (gameObject.IsEnabled())
		{
			auto& position = gameObjectView.get<Components::Position>(entity);
			glm::vec3 pos = position.Get();
			glm::vec3 posAdj = glm::vec3(0.0, 0.01, 0.0);
			position.Set(pos + posAdj);
		}
	}

	/*auto containerView = registry.view<Components::Container, Components::Scale>();
	for (auto entity : containerView)
	{
		auto& container = containerView.get<Components::Container>(entity);
		if (container.IsEnabled())
		{
			auto& scale = containerView.get<Components::Scale>(entity);
			glm::vec2 pod = container.GetPercentageOfDimensions();

			scale.Set(glm::vec2(displayData.x * pod.x, displayData.y * pod.y));
		}
	}*/
}

void render(entt::registry& registry, double normalizedTime)
{
	// Views get created when queried. It exposes internal data structures of the registry to itself.
	// Views are cheap to make/destroy.
	// Views are meant to be temporary; don't store them after


	Shader* shader = shaders["model"]; // Need to look at this a bit closer.

	// We're assuming we just have one here, and that it's always enabled, even though we're checking for it.
	// We should only have one of either an Orthographic Camera, or a Perspective Camera.
	auto orthographicCameraView = registry.view<Components::OrthographicCamera>();
	for (auto entity : orthographicCameraView)
	{
		auto& camera = orthographicCameraView.get<Components::OrthographicCamera>(entity);
		if (camera.IsEnabled())
		{
			camera.UpdateProjectionMatrix();

			shader->use();
			shader->setMat4("projection", camera.GetProjectionMatrix());
			shader->setMat4("view", camera.GetViewMatrix());
		}
	}
	auto perspectiveCameraView = registry.view<Components::PerspectiveCamera>();
	for (auto entity : perspectiveCameraView)
	{
		auto& camera = perspectiveCameraView.get<Components::PerspectiveCamera>(entity);
		if (camera.IsEnabled())
		{
			camera.UpdateProjectionMatrix();

			shader->use();
			shader->setMat4("projection", camera.GetProjectionMatrix());
			shader->setMat4("view", camera.GetViewMatrix());
		}
	}



	auto renderView = registry.view<Components::Renderable, Components::Position, Components::Scale>();
	for (auto entity : renderView)
	{
		auto& render = renderView.get<Components::Renderable>(entity);
		auto& position = renderView.get<Components::Position>(entity);
		auto& scale = renderView.get<Components::Scale>(entity);
			
		if (render.IsEnabled() && position.IsEnabled())
		{
			
			glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity Matrix
			modelMatrix = glm::translate(modelMatrix, position.Get());
			modelMatrix = glm::scale(modelMatrix, scale.Get());

			shader->setMat4("model", modelMatrix);
			render.Draw(*shader);
		}
	}
}

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
			registry.emplace<Components::Renderable>(cell, Model("./data/block/grey.obj"));
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

int main()
{
	if (!glfwInit())
	{
		// Initialization failed.
		return -1;
	}

	glfwSetErrorCallback(glfwErrorCallback);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	// Might make sense not to explicitly get a context version, for what we're doing?
	// GLFW supports borderless fullscreeen as well. Look later maybe.
	GLFWwindow* window = glfwCreateWindow(displayData.x, displayData.y, displayData.title.c_str(), NULL, NULL);
	if (!window)
	{
		// Window creation or OpenGL Context Creation failed
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// Doing more GL setup stuff. Comment out some stuff from tutorial to do later.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);
	// glfwSetScrollCallback(window, scroll_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	//stbi_set_flip_vertically_on_load(true);

	// Begin ECS
	entt::registry registry;

	const auto camera = registry.create();
	registry.emplace<Components::OrthographicCamera>(camera, glm::vec3(0.0f, 0.0f, 3.0f));
	//registry.emplace<Components::PerspectiveCamera>(camera, glm::vec3(0.0f, 0.0f, 3.0f));
	/*
	const auto model = registry.create();
	//registry.emplace<GameObjectComponent>(glm::vec3(0.0f, 0.0f, 0.0f)); // TODO
	registry.emplace<Components::Renderable>(model, Model("./data/box/cube.obj"));
	registry.emplace<Components::Position>(model, glm::vec3(50.0f, 50.0f, 0.0f));
	registry.emplace<Components::Scale>(model, glm::vec3(100.0f, 100.0f, 1.0f));
	registry.emplace<Components::GameObject>(model);*/
	/*
	const auto model2 = registry.create();
	registry.emplace<Components::Renderable>(model2, Model("./data/box/cube.obj"));
	registry.emplace<Components::Position>(model2, glm::vec3(300.0f, 200.0f, 0.0f));
	registry.emplace<Components::Scale>(model2, glm::vec3(50.0f, 50.0f, 1.0f));
	registry.emplace<Components::GameObject>(model2);*/

	const auto playArea = registry.create();
	//registry.emplace<Components::Renderable>(playArea, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	//registry.emplace<Components::Position>(playArea, glm::vec3(0.0f, 0.0f, 0.0f));
	//registry.emplace<Components::Scale>(playArea, glm::vec3(1.0f, 1.0f, 1.0f));
	registry.emplace<Components::Position>(playArea, glm::vec3(displayData.x/2, displayData.y/2, 0.0f));
	registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(2, 2), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, "Play Area");

	BuildGrid(registry, playArea);

	/*
	auto& container2 = registry.get<Components::Container2>(playArea);
	// We want a copy of this to be stored in this scope, because the component reference may change without warning.
	// We could also just store the vector coordinate. Either way.
	Components::Position parentPosition = registry.get<Components::Position>(playArea);

	const auto grid1 = registry.create();
	registry.emplace<Components::Coordinate>(grid1, glm::uvec2(0, 0));
	registry.emplace<Components::Cell>(grid1, playArea);
	registry.emplace<Components::Tag>(grid1, "Grid1");
	registry.emplace<Components::Scale>(grid1, container2.GetCellDimensions3());
	registry.emplace<Components::Position>(grid1, container2.GetCellPosition3(parentPosition.Get(), glm::uvec2(0, 0)));
	registry.emplace<Components::Renderable>(grid1, Model("./data/block/grey.obj"));
	
	const auto grid2 = registry.create();
	registry.emplace<Components::Coordinate>(grid2, glm::uvec2(0, 1));
	registry.emplace<Components::Cell>(grid2, playArea);
	registry.emplace<Components::Tag>(grid2, "Grid2");
	registry.emplace<Components::Scale>(grid2, container2.GetCellDimensions3());
	registry.emplace<Components::Position>(grid2, container2.GetCellPosition3(parentPosition.Get(), glm::uvec2(0, 1))); // Parent Position abruptly corrupt?
	registry.emplace<Components::Renderable>(grid2, Model("./data/block/grey.obj"));

	const auto grid3 = registry.create();
	registry.emplace<Components::Coordinate>(grid3, glm::uvec2(1, 0));
	registry.emplace<Components::Cell>(grid3, playArea);
	registry.emplace<Components::Tag>(grid3, "Grid3");
	registry.emplace<Components::Scale>(grid3, container2.GetCellDimensions3());
	registry.emplace<Components::Position>(grid3, container2.GetCellPosition3(parentPosition.Get(), glm::uvec2(1, 0)));
	registry.emplace<Components::Renderable>(grid3, Model("./data/block/grey.obj"));
	
	const auto grid4 = registry.create();
	registry.emplace<Components::Coordinate>(grid4, glm::uvec2(1, 1));
	registry.emplace<Components::Cell>(grid4, playArea);
	registry.emplace<Components::Tag>(grid4, "Grid4");
	registry.emplace<Components::Scale>(grid4, container2.GetCellDimensions3());
	registry.emplace<Components::Position>(grid4, container2.GetCellPosition3(parentPosition.Get(), glm::uvec2(1, 1)));
	registry.emplace<Components::Renderable>(grid4, Model("./data/block/grey.obj"));
	*/

	// G1 G2
	// G3 G4
	/*
	auto& grid1cell = registry.get<Components::Cell>(grid1);
	
	grid1cell.SetSouth(grid3);
	grid1cell.SetEast(grid2);

	auto& grid2cell = registry.get<Components::Cell>(grid2);
	grid2cell.SetWest(grid1);
	grid2cell.SetSouth(grid4);

	auto& grid3cell = registry.get<Components::Cell>(grid3);
	grid3cell.SetNorth(grid1);
	grid3cell.SetEast(grid4);

	auto& grid4cell = registry.get<Components::Cell>(grid4);
	grid4cell.SetNorth(grid2);
	grid4cell.SetWest(grid3);
	*/

	// Testing

	auto gridCellView = registry.view<Components::Cell, Components::Coordinate, Components::Tag, Components::Scale, Components::Position>();
	for (auto entity : gridCellView)
	{
		auto& cell = gridCellView.get<Components::Cell>(entity);
		auto& coordinate = gridCellView.get<Components::Coordinate>(entity);
		auto& tag = gridCellView.get<Components::Tag>(entity);

		auto& scale = gridCellView.get<Components::Scale>(entity);
		auto& position = gridCellView.get<Components::Position>(entity);
		if (cell.IsEnabled() && coordinate.IsEnabled())
		{
			glm::uvec2 coord = coordinate.Get();

			cout << "Cell(" << coordinate << "): " << tag.Get() << endl;
			auto parent = registry.get<Components::Tag>(cell.GetParent()); // Assume this succeeds for now.
			cout << "Parent(" << parent.Get() << ")" << endl;
			Components::Coordinate* pCoord = NULL;
			if (cell.GetNorth() != entt::null)
			{
				pCoord = &registry.get<Components::Coordinate>(cell.GetNorth());
				cout << "North(" << to_string(*pCoord) << ")" << endl;
				pCoord = NULL;
			}
			if (cell.GetSouth() != entt::null)
			{
				pCoord = &registry.get<Components::Coordinate>(cell.GetSouth());
				cout << "South(" << to_string(*pCoord) << ")" << endl;
				pCoord = NULL;
			}
			if (cell.GetWest() != entt::null)
			{
				pCoord = &registry.get<Components::Coordinate>(cell.GetWest());
				cout << "West(" << to_string(*pCoord) << ")" << endl;
				pCoord = NULL;
			}
			if (cell.GetEast() != entt::null)
			{
				pCoord = &registry.get<Components::Coordinate>(cell.GetEast());
				cout << "East(" << to_string(*pCoord) << ")" << endl;
				pCoord = NULL;
			}
		}
	}

	/*registry.emplace<Components::Container>(playArea, glm::vec2(0.4, 0.8), glm::uvec2(10, 20), glm::uvec2(25, 25));
	auto temp = registry.get<Components::Container>(playArea);
	for (int i = 0; i < temp.GetGridDimensions().x; i++)
	{
		for (int j = 0; j < temp.GetGridDimensions().y; j++)
		{
			const auto gridCoordinate = registry.create();
			glm::vec2 pos = temp.GetGridSquareCoordinates(glm::uvec2(i, j));
			//if (i % 2)
				//pos.y =- 20;
			//registry.emplace<Components::Renderable>(gridCoordinate, Model("./data/block/block.obj"));
			glm::vec3 pos2 = glm::vec3(displayData.x / 2 + pos.x, displayData.y / 2 + pos.y, 0.0f);
			registry.emplace<Components::Position>(gridCoordinate, pos2);
			registry.emplace<Components::Scale>(gridCoordinate, glm::vec3(25.0f, 25.0f, 1.0f));

			temp.m_grid.push_back(gridCoordinate);
		}
	}

	glm::uvec2 dim = temp.GetGridSquareDimensions(registry, playArea);*/
	
	// End ECS

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// Do one-time OpenGL things here.
	Shader* shader = RetrieveShader("model", "./data/shaders/1.model_loading.vs", "./data/shaders/1.model_loading.fs");
	//Model ourModel("./data/box/cube.obj");
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GameTime::Initialize(glfwGetTime());

	while (!glfwWindowShouldClose(window))
	{
		double currentFrameTime = glfwGetTime();
		double deltaTime = currentFrameTime - GameTime::lastFrameTime;
		GameTime::lastFrameTime = currentFrameTime;
		GameTime::accumulator += deltaTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		while (GameTime::accumulator >= GameTime::fixedDeltaTime)
		{
			// Update game logic for ECS
			update(registry);
			GameTime::accumulator -= GameTime::fixedDeltaTime;
		}
		// Update render objects.
		render(registry, GameTime::accumulator / GameTime::fixedDeltaTime);

		glfwSwapBuffers(window);
		glfwPollEvents(); // Windows needs to do things with the window too!
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void glfwErrorCallback(int error, const char* description)
{
	std::cout << "Error " << error << ": " << description << endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}