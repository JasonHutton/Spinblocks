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

#include "Input/InputHandler.h"
#include "Input/GameInput.h"

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

InputHandler input;

const Components::Coordinate& GetCoordinateOfEntity(entt::registry& registry, const entt::entity& entity)
{
	if(entity == entt::null)
		throw std::runtime_error("Entity is NULL!");

	if (!registry.has<Components::Coordinate>(entity))
		throw std::runtime_error("Entity does not have component!");

	return registry.get<Components::Coordinate>(entity);
}

const Components::Cell& GetCellOfEntity(entt::registry& registry, const entt::entity& entity)
{
	if (entity == entt::null)
		throw std::runtime_error("Entity is NULL!");

	if (!registry.has<Components::Cell>(entity))
		throw std::runtime_error("Entity does not have component!");

	return registry.get<Components::Cell>(entity);
}

Components::Cell& GetCellAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate)
{
	auto cellView = registry.view<Components::Cell, Components::Coordinate>();
	for (auto entity : cellView)
	{
		auto& cell = cellView.get<Components::Cell>(entity);
		auto& cellCoordinate = registry.get<Components::Coordinate>(entity);

		if (cell.IsEnabled() && cellCoordinate.IsEnabled())
		{
			if (cellCoordinate == coordinate)
			{
				// We're short circuiting here on the first match. We probably want to check for container first. (eg: When we've got multiple reference containers in use.)
				return cell;
			}
		}
	}

	throw std::runtime_error("Unable to find Cell at coordinates!");
}

const Components::Block& GetBlockAtCoordinates(entt::registry& registry, const Components::Coordinate& coordinate)
{
	auto blockView = registry.view<Components::Block>();
	for (auto entity : blockView)
	{
		auto& block = blockView.get<Components::Block>(entity);

		if (block.IsEnabled())
		{
			auto& blockCoordinate = registry.get<Components::Coordinate>(block.Get());
			if (blockCoordinate == coordinate)
			{
				// We're short circuiting here on the first match. We probably want to check for container first. (eg: When we've got multiple reference containers in use.)
				return block;
			}
		}
	}

	throw std::runtime_error("Unable to find Block at coordinates!");
}

// We'll want to spawn whole tetrominoes later, not just blocks.
void SpawnBlock(entt::registry& registry, const Components::Coordinate& spawnCoordinate)
{
	auto containerView = registry.view<Components::Container2, Components::Tag>();
	for (auto entity : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(entity);
		auto& tag = containerView.get<Components::Tag>(entity); // We'll be wanting to check which container we're working with later. (eg: Play Area, Hold, Preview, (which play area?))
		if (container2.IsEnabled() && tag.IsEnabled())
		{
			// Remove all existing controllable blocks.
			// We probably want this more where one locks down, not here, but for now this is fine.
			auto blockView = registry.view<Components::Block, Components::Controllable>();
			for (auto block : blockView)
			{
				registry.remove_if_exists<Components::Controllable>(block);
			}
			Components::Container2 container2 = registry.get<Components::Container2>(entity);
			Components::Position parentPosition = registry.get<Components::Position>(entity);

			const auto piece1 = registry.create();
			registry.emplace<Components::Coordinate>(piece1, spawnCoordinate);
			registry.emplace<Components::Position>(piece1);
			registry.emplace<Components::DerivePositionFromCoordinates>(piece1, entity);
			registry.emplace<Components::Scale>(piece1, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(piece1, Model("./data/block/yellow.obj"));
			registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), registry.get<Components::Coordinate>(piece1));
			//registry.emplace<Components::Moveable>(piece1, registry.get<Components::Coordinate>(piece1), Components::Coordinate(glm::uvec2(1, 0)));// registry.get<Components::Coordinate>(piece1));
			registry.emplace<Components::Controllable>(piece1, entity);
			registry.emplace<Components::Block>(piece1, entity);
		}
	}
}

bool CanOccupyCell(entt::registry& registry, const entt::entity& cellEntity)
{
	if (cellEntity == entt::null)
		return false;

	if (!registry.has<Components::Cell>(cellEntity))
		return false;

	if (!registry.has<Components::Coordinate>(cellEntity))
		return false;

	auto& cell = registry.get<Components::Cell>(cellEntity);
	auto& cellCoordinate = registry.get<Components::Coordinate>(cellEntity);

	if (!cell.IsEnabled() || !cellCoordinate.IsEnabled())
		return false;

	auto blockView = registry.view<Components::Block, Components::Coordinate>();
	for (auto blockEntity : blockView)
	{
		auto& block = registry.get<Components::Block>(blockEntity);
		auto& blockCoordinate = registry.get<Components::Coordinate>(blockEntity);

		if (block.IsEnabled() && blockCoordinate.IsEnabled())
		{
			if (blockCoordinate == cellCoordinate)
			{
				// We're short circuiting here on the first match. We probably want to check for container first. (eg: When we've got multiple reference containers in use.)
				return false;
			}
		}
	}

	return true;
}

const int PlayAreaWidth = 10;
const double KeyRepeatDelay = 0.3; // Delay before starting to repeat.
const double KeyRepeatRate = 0.5 / PlayAreaWidth; // Delay between repeats.

void processinput(GLFWwindow* window, entt::registry& registry, double currentFrameTime)
{
	// clear last gameinput state
	GameInput::clearState();

	// Check all potentially-pressed keys
	for (auto& keyState : input.GetAllKeyStates())
	{
		keyState.second.prevKeyDown = keyState.second.keyDown;
		if (glfwGetKey(window, keyState.first) == GLFW_PRESS)
		{
			keyState.second.keyDown = true;
		}
		else
		{
			keyState.second.keyDown = false;
		}
	}

	// Check all bound controls, as single buttons
	for (auto& keyState : input.GetAllKeyStates())
	{
		// Manage keydown/keyup begin times
		if (keyState.second.keyDown == true)
		{
			keyState.second.currentKeyUpBeginTime = 0.0;

			if (keyState.second.prevKeyDown == false)
				keyState.second.currentKeyDownBeginTime = currentFrameTime;
		}
		else // keyDown == false
		{
			keyState.second.currentKeyDownBeginTime = 0.0;

			if (keyState.second.prevKeyDown == true)
				keyState.second.currentKeyUpBeginTime = currentFrameTime;
		}


		// If the bound control is being pressed....
		if (keyState.second.keyDown == true)
		{
			// See if a bound control has a User Button associated with it.
			ContextControl cc = input.GetControl(keyState.first);
			// Do what the context control->User Button says to do.
			switch (cc.GetControl("")) // Default context.
			{
			case KeyInput::usercmdButton_t::UB_FORCE_QUIT:
				glfwSetWindowShouldClose(window, true);
				break;
			case KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_1:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				// Spawn a block in column 1
				cout << "Key 1 is being triggered." << endl;

				SpawnBlock(registry, glm::uvec2(0, 19));

				break;
			}
			case KeyInput::usercmdButton_t::UB_MOVE_LEFT:
			{
				if (keyState.second.prevKeyDown == true)
				{
					if (keyState.second.currentKeyDownBeginTime + KeyRepeatDelay >= currentFrameTime)
						break;

					if (keyState.second.lastKeyDownRepeatTime + KeyRepeatRate >= currentFrameTime)
						break;
				}
				keyState.second.lastKeyDownRepeatTime = currentFrameTime;

				auto controllableView = registry.view<Components::Controllable, Components::Moveable>();
				auto cellView = registry.view<Components::Cell, Components::Coordinate>();
				for (auto entity1 : controllableView)
				{
					auto& controllable = controllableView.get<Components::Controllable>(entity1);
					auto& moveable = controllableView.get<Components::Moveable>(entity1);

					if (controllable.IsEnabled() && moveable.IsEnabled())
					{
						for (auto entity2 : cellView)
						{
							auto& cell = cellView.get<Components::Cell>(entity2);
							auto& coordinate = cellView.get<Components::Coordinate>(entity2);

							if (cell.IsEnabled() && coordinate.IsEnabled())
							{
								if (controllable.Get() == cell.GetParent())
								{
									if (moveable.GetCurrentCoordinate() == coordinate)
									{
										try
										{
											if (CanOccupyCell(registry, cell.GetWest()))
											{
												moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, cell.GetWest()));
											}
										}
										catch (std::runtime_error ex)
										{
											cerr << ex.what() << endl;
										}
									}
								}
							}
						}
					}
				}
				break;
			}
			case KeyInput::usercmdButton_t::UB_MOVE_RIGHT:
			{
				if (keyState.second.prevKeyDown == true)
				{
					if (keyState.second.currentKeyDownBeginTime + KeyRepeatDelay >= currentFrameTime)
						break;

					if (keyState.second.lastKeyDownRepeatTime + KeyRepeatRate >= currentFrameTime)
						break;
				}
				keyState.second.lastKeyDownRepeatTime = currentFrameTime;

				auto controllableView = registry.view<Components::Controllable, Components::Moveable>();
				auto cellView = registry.view<Components::Cell, Components::Coordinate>();
				for (auto entity1 : controllableView)
				{
					auto& controllable = controllableView.get<Components::Controllable>(entity1);
					auto& moveable = controllableView.get<Components::Moveable>(entity1);

					if (controllable.IsEnabled() && moveable.IsEnabled())
					{
						for (auto entity2 : cellView)
						{
							auto& cell = cellView.get<Components::Cell>(entity2);
							auto& coordinate = cellView.get<Components::Coordinate>(entity2);

							if (cell.IsEnabled() && coordinate.IsEnabled())
							{
								if (controllable.Get() == cell.GetParent())
								{
									if (moveable.GetCurrentCoordinate() == coordinate)
									{
										try
										{
											if (CanOccupyCell(registry, cell.GetEast()))
											{
												moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, cell.GetEast()));
											}
										}
										catch (std::runtime_error ex)
										{
											cerr << ex.what() << endl;
										}
									}
								}
							}
						}
					}
				}
				break;
			}
			case KeyInput::usercmdButton_t::UB_NONE:
			default:
				break;
			}

			/*switch (cc.GetControl("Menu")) // Some other context.
			{

			}*/
		}
	}
}


void preupdate(entt::registry& registry, double currentFrameTime)
{

}

const double FallSpeed = 1.0;
double lastFallUpdate = 0.0;

void update(entt::registry& registry, double currentFrameTime)
{
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

	if (currentFrameTime >= lastFallUpdate + FallSpeed)
	{
		lastFallUpdate = currentFrameTime;

		auto moveableViewx = registry.view<Components::Moveable, Components::Coordinate>();
		for (auto entity : moveableViewx)
		{
			auto& moveable = moveableViewx.get<Components::Moveable>(entity);
			auto& coordinate = moveableViewx.get<Components::Coordinate>(entity);
			if (moveable.IsEnabled() && coordinate.IsEnabled())
			{
				auto& cell = GetCellAtCoordinates(registry, moveable.GetCurrentCoordinate());
				if (cell.IsEnabled())
				{
					if (CanOccupyCell(registry, cell.GetSouth()))
					{
						moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, cell.GetSouth()));
					}
				}
			}
		}
	}

	auto moveableView = registry.view<Components::Moveable, Components::Coordinate>();
	for (auto entity : moveableView)
	{
		auto& moveable = moveableView.get<Components::Moveable>(entity);
		auto& coordinate = moveableView.get<Components::Coordinate>(entity);
		if (moveable.IsEnabled() && coordinate.IsEnabled())
		{
			if (moveable.GetCurrentCoordinate() != moveable.GetDesiredCoordinate())
			{
				// Need to detect if a move is allowed before permitting it.
				coordinate = moveable.GetDesiredCoordinate();
				moveable.SetCurrentCoordinate(coordinate);
			}
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
void postupdate(entt::registry& registry, double currentFrameTime)
{

}


void prerender(entt::registry& registry, double normalizedTime)
{
	auto scaleView = registry.view<Components::ScaleToCellDimensions, Components::Scale, Components::Cell>();
	for (auto entity : scaleView)
	{
		auto& scaleToCellDimensions = scaleView.get<Components::ScaleToCellDimensions>(entity);
		auto& scale = scaleView.get<Components::Scale>(entity);
		auto& cell = scaleView.get<Components::Cell>(entity);
		
		if (scaleToCellDimensions.IsEnabled() && scale.IsEnabled() && cell.IsEnabled())
		{
			Components::Container2 container2 = registry.get<Components::Container2>(cell.GetParent());
			scale.Set(container2.GetCellDimensions3());
		}
	}

	auto positionFromParentView = registry.view<Components::DerivePositionFromParent, Components::Position>();
	for (auto entity : positionFromParentView)
	{
		auto& derivePositionFromParent = positionFromParentView.get<Components::DerivePositionFromParent>(entity);
		auto& position = positionFromParentView.get<Components::Position>(entity);

		if (derivePositionFromParent.IsEnabled() && position.IsEnabled())
		{
			Components::Position parentPosition = registry.get<Components::Position>(derivePositionFromParent.Get());

			position.Set(parentPosition.Get());
		}
	}

	// We're getting a position based upon the coordinates here, relative to the parent entity's position, as coordinates have no meaning without that parent container.
	auto derivedPositionView = registry.view<Components::DerivePositionFromCoordinates, Components::Position, Components::Coordinate>();
	for (auto entity : derivedPositionView)
	{
		auto& derivePositionFromCoordinates = derivedPositionView.get<Components::DerivePositionFromCoordinates>(entity);
		auto& position = derivedPositionView.get<Components::Position>(entity);
		auto& coordinates = derivedPositionView.get<Components::Coordinate>(entity);

		if (derivePositionFromCoordinates.IsEnabled() && position.IsEnabled() && coordinates.IsEnabled())
		{
			Components::Position parentPosition = registry.get<Components::Position>(derivePositionFromCoordinates.Get());
			Components::Container2 container2 = registry.get<Components::Container2>(derivePositionFromCoordinates.Get());

			position.Set(container2.GetCellPosition3(parentPosition.Get(), coordinates.Get()));
		}
	}
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
void postrender(entt::registry& registry, double normalizedTime)
{

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
			registry.emplace<Components::Scale>(cell);
			registry.emplace<Components::Position>(cell);
			registry.emplace<Components::DerivePositionFromCoordinates>(cell, parentEntity);
			registry.emplace<Components::Renderable>(cell, Model("./data/block/grey.obj"));
			registry.emplace<Components::ScaleToCellDimensions>(cell, parentEntity);
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
					cell1.SetNorth(entity2);
				}

				if (coordinate1.Get().y - 1 == coordinate2.Get().y)
				{
					cell1.SetSouth(entity2);
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

	GameInput::setVerticalAxis(0);
	GameInput::setHorizontalAxis(0);

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
	registry.emplace<Components::Renderable>(playArea, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	//registry.emplace<Components::Position>(playArea, glm::vec3(0.0f, 0.0f, 0.0f));
	registry.emplace<Components::Scale>(playArea, glm::vec2(25*10, 25*20)); // celldimensions * gridwidth or height
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x/2, displayData.y/2));
	//registry.emplace<Components::Scale>(playArea);
	registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, "Play Area");

	BuildGrid(registry, playArea);
	/*
	Components::Container2 container2 = registry.get<Components::Container2>(playArea);
	Components::Position parentPosition = registry.get<Components::Position>(playArea);

	const auto piece1 = registry.create();
	registry.emplace<Components::Coordinate>(piece1, glm::uvec2(0, 0));
	registry.emplace<Components::Position>(piece1);
	registry.emplace<Components::DerivePositionFromCoordinates>(piece1, playArea);
	registry.emplace<Components::Scale>(piece1, container2.GetCellDimensions3());
	registry.emplace<Components::Renderable>(piece1, Model("./data/block/yellow.obj"));

	
	const auto piece2 = registry.create();
	registry.emplace<Components::Coordinate>(piece2, glm::uvec2(1, 0));
	registry.emplace<Components::Position>(piece2);
	registry.emplace<Components::DerivePositionFromCoordinates>(piece2, playArea);
	registry.emplace<Components::Scale>(piece2, container2.GetCellDimensions3());
	registry.emplace<Components::Renderable>(piece2, Model("./data/block/lightblue.obj"));
	
	const auto piece3 = registry.create();
	registry.emplace<Components::Coordinate>(piece3, glm::uvec2(2, 0));
	registry.emplace<Components::Position>(piece3);
	registry.emplace<Components::DerivePositionFromCoordinates>(piece3, playArea);
	registry.emplace<Components::Scale>(piece3, container2.GetCellDimensions3());
	registry.emplace<Components::Renderable>(piece3, Model("./data/block/purple.obj"));

	const auto piece4 = registry.create();
	registry.emplace<Components::Coordinate>(piece4, glm::uvec2(3, 0));
	registry.emplace<Components::Position>(piece4);
	registry.emplace<Components::DerivePositionFromCoordinates>(piece4, playArea);
	registry.emplace<Components::Scale>(piece4, container2.GetCellDimensions3());
	registry.emplace<Components::Renderable>(piece4, Model("./data/block/orange.obj"));

	const auto piece5 = registry.create();
	registry.emplace<Components::Coordinate>(piece5, glm::uvec2(4, 0));
	registry.emplace<Components::Position>(piece5);
	registry.emplace<Components::DerivePositionFromCoordinates>(piece5, playArea);
	registry.emplace<Components::Scale>(piece5, container2.GetCellDimensions3());
	registry.emplace<Components::Renderable>(piece5, Model("./data/block/darkblue.obj"));

	const auto piece6 = registry.create();
	registry.emplace<Components::Coordinate>(piece6, glm::uvec2(5, 0));
	registry.emplace<Components::Position>(piece6);
	registry.emplace<Components::DerivePositionFromCoordinates>(piece6, playArea);
	registry.emplace<Components::Scale>(piece6, container2.GetCellDimensions3());
	registry.emplace<Components::Renderable>(piece6, Model("./data/block/green.obj"));

	const auto piece7 = registry.create();
	registry.emplace<Components::Coordinate>(piece7, glm::uvec2(6, 0));
	registry.emplace<Components::Position>(piece7);
	registry.emplace<Components::DerivePositionFromCoordinates>(piece7, playArea);
	registry.emplace<Components::Scale>(piece7, container2.GetCellDimensions3());
	registry.emplace<Components::Renderable>(piece7, Model("./data/block/red.obj"));
	*/
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

		processinput(window, registry, currentFrameTime);
		
		while (GameTime::accumulator >= GameTime::fixedDeltaTime)
		{
			// Update game logic for ECS
			preupdate(registry, currentFrameTime);
			update(registry, currentFrameTime);
			postupdate(registry, currentFrameTime);

			GameTime::accumulator -= GameTime::fixedDeltaTime;
		}
		// Update render objects.
		prerender(registry, GameTime::accumulator / GameTime::fixedDeltaTime);
		render(registry, GameTime::accumulator / GameTime::fixedDeltaTime);
		postrender(registry, GameTime::accumulator / GameTime::fixedDeltaTime);

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