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

void PlaceEdgeMarker(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& markerCoordinate, entt::entity adjacentEntity, const moveDirection_t& dir)
{
	auto containerView = registry.view<Components::Container2, Components::Tag>();
	for (auto entity : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(entity);
		auto& containerTag2 = containerView.get<Components::Tag>(entity);

		if (container2.IsEnabled() && containerTag2.IsEnabled())
		{
			if (containerTag2.Get() != containerTag)
				continue;

			entt::entity cellEnt = GetCellAtCoordinates2(registry, markerCoordinate);

			if (cellEnt == entt::null)
				continue;

			const auto marker = registry.create();
			registry.emplace<Components::CellLink>(marker, entity, adjacentEntity, dir);
			registry.emplace<Components::Coordinate>(marker, markerCoordinate.GetParent(), markerCoordinate.Get());
			registry.emplace<Components::Position>(marker);
			registry.emplace<Components::DerivePositionFromCoordinates>(marker);
			registry.emplace<Components::Scale>(marker, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(marker, Components::renderLayer_t::RL_MARKER, Model("./data/block/green.obj"));
		}
	}
}

//void PlaceEdgeMarker2(entt::registry& registry, const std::string& originTag, const Components::Coordinate& originCoordinate, entt::entity destinationTagadjacentEntity, const moveDirection_t& dir)

void PlaceMarker(entt::registry& registry, const std::string& containerTag, const std::string& markerTag, const Components::Coordinate& markerCoordinate)
{
	auto containerView = registry.view<Components::Container2, Components::Tag>();
	for (auto entity : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(entity);
		auto& containerTag2 = containerView.get<Components::Tag>(entity);

		if (container2.IsEnabled() && containerTag2.IsEnabled())
		{
			if (containerTag2.Get() != containerTag)
				continue;

			entt::entity cellEnt = GetCellAtCoordinates2(registry, markerCoordinate);

			if (cellEnt == entt::null)
				continue;

			const auto marker = registry.create();
			registry.emplace<Components::Marker>(marker, entity);
			registry.emplace<Components::Coordinate>(marker, markerCoordinate.GetParent(), markerCoordinate.Get());
			registry.emplace<Components::Position>(marker);
			registry.emplace<Components::DerivePositionFromCoordinates>(marker);
			registry.emplace<Components::Scale>(marker, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(marker, Components::renderLayer_t::RL_MARKER, Model("./data/block/red.obj"));
			registry.emplace<Components::Tag>(marker, markerTag);
		}
	}
}

// Not actually using containerTag here for the moment. May make more sense to just have it detect which tag, as it does currently.
// As we'll only really have one piece moving at a time, probably fine. Change later if not.
void MovePiece(entt::registry& registry, const movePiece_t& movePiece)
{
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
				//std::string tagOfContainerEntity = FindTagOfContainerEntity(registry, cell.GetParent());

				if (cell.IsEnabled() && coordinate.IsEnabled())
				{
					if (controllable.Get() == cell.GetParent())
					{
						if (moveable.GetCurrentCoordinate() == coordinate)
						{
							try
							{
								switch (movePiece)
								{
								case movePiece_t::MOVE_LEFT:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, moveDirection_t::WEST, 1)));
									break;
								case movePiece_t::MOVE_RIGHT:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, moveDirection_t::EAST, 1)));
									break;
								case movePiece_t::MOVE_UP:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, moveDirection_t::NORTH, 1)));
									moveable.SetMovementState(Components::movementStates_t::DEBUG_MOVE_UP);
									break;
								case movePiece_t::SOFT_DROP:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, moveDirection_t::SOUTH, 1)));
									moveable.SetMovementState(Components::movementStates_t::SOFT_DROP);
									break;
								case movePiece_t::HARD_DROP:
								{
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, moveDirection_t::SOUTH, PlayAreaHeight + BufferAreaHeight)));
									moveable.SetMovementState(Components::movementStates_t::HARD_DROP); // Hard drop state even if we're not able to move. We did trigger this.
									break;
								}
								default:
									break;
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
}

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


				SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(0, 19)));

				break;
			}
			case KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_2:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				SpawnBlock(registry, GetTagFromContainerType(containerType_t::BAG_AREA), Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::BAG_AREA)), glm::uvec2(0, 15)));

				break;
			}
			case KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_3:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(4, 18)));

				break;
			}
			case KeyInput::usercmdButton_t::UB_DEBUG_MOVE_UP:
			{
				auto controllableView = registry.view<Components::Controllable, Components::Moveable>();
				for (auto entity : controllableView)
				{
					auto& controllable = controllableView.get<Components::Controllable>(entity);
					auto& moveable = controllableView.get<Components::Moveable>(entity);

					if (controllable.IsEnabled() && moveable.IsEnabled())
					{
						moveable.SetMovementState(Components::movementStates_t::DEBUG_MOVE_UP);
					}
				}

				if (keyState.second.prevKeyDown == true)
				{
					if (keyState.second.currentKeyDownBeginTime + KeyRepeatDelay >= currentFrameTime)
						break;

					if (keyState.second.lastKeyDownRepeatTime + KeyRepeatRate >= currentFrameTime)
						break;
				}
				keyState.second.lastKeyDownRepeatTime = currentFrameTime;

				MovePiece(registry, movePiece_t::MOVE_UP);

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

				MovePiece(registry, movePiece_t::MOVE_LEFT);

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

				MovePiece(registry, movePiece_t::MOVE_RIGHT);
				break;
			}
			case KeyInput::usercmdButton_t::UB_SOFT_DROP:
			{
				auto controllableView = registry.view<Components::Controllable, Components::Moveable>();
				for (auto entity : controllableView)
				{
					auto& controllable = controllableView.get<Components::Controllable>(entity);
					auto& moveable = controllableView.get<Components::Moveable>(entity);

					if (controllable.IsEnabled() && moveable.IsEnabled())
					{
						moveable.SetMovementState(Components::movementStates_t::FALL);
					}
				}

				if (keyState.second.prevKeyDown == true)
				{
					if (keyState.second.currentKeyDownBeginTime + KeyRepeatDelay >= currentFrameTime)
						break;

					if (keyState.second.lastKeyDownRepeatTime + KeyRepeatRate >= currentFrameTime)
						break;
				}
				keyState.second.lastKeyDownRepeatTime = currentFrameTime;

				MovePiece(registry, movePiece_t::SOFT_DROP);
				break;
			}
			case KeyInput::usercmdButton_t::UB_HARD_DROP:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				auto controllableView = registry.view<Components::Controllable, Components::Moveable>();
				for (auto entity : controllableView)
				{
					auto& controllable = controllableView.get<Components::Controllable>(entity);
					auto& moveable = controllableView.get<Components::Moveable>(entity);

					if (controllable.IsEnabled() && moveable.IsEnabled())
					{
						moveable.SetMovementState(Components::movementStates_t::FALL);
					}
				}

				MovePiece(registry, movePiece_t::HARD_DROP);
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

	Systems::FallingSystem(registry, currentFrameTime);
	Systems::MovementSystem(registry, currentFrameTime);
	Systems::StateChangeSystem(registry, currentFrameTime);
	Systems::PatternSystem(registry, PlayAreaWidth, currentFrameTime);
	Systems::EliminateSystem(registry, currentFrameTime);

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

			position.Set(parentPosition.Get() + derivePositionFromParent.GetOffset());
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
			entt::entity deriveCoordinatesFrom = derivePositionFromCoordinates.Get();
			if (deriveCoordinatesFrom == entt::null)
			{
				deriveCoordinatesFrom = coordinates.GetParent();
			}
			Components::Position parentPosition = registry.get<Components::Position>(deriveCoordinatesFrom);
			Components::Container2 container2 = registry.get<Components::Container2>(deriveCoordinatesFrom);

			position.Set(container2.GetCellPosition3(parentPosition.Get(), coordinates.Get()) + derivePositionFromCoordinates.GetOffset());
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
	for (int i = Components::renderLayer_t::RL_MIN+1; i < Components::renderLayer_t::RL_MAX; i++)
	{
		for (auto entity : renderView)
		{
			auto& render = renderView.get<Components::Renderable>(entity);
			if (render.GetLayer() != i)
				continue;

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
}
void postrender(entt::registry& registry, double normalizedTime)
{

}

glm::ivec2 GetConnectionLine(const glm::uvec2& dimensions, const moveDirection_t& direction)
{
	switch (direction)
	{
	case moveDirection_t::EAST:
		return glm::ivec2(dimensions.x, -1);
	case moveDirection_t::NORTH:
		return glm::ivec2(-1, dimensions.y);
	case moveDirection_t::SOUTH:
		return glm::ivec2(-1, 0);
	case moveDirection_t::WEST:
		return glm::ivec2(0, -1);
	}

	throw std::runtime_error("GetConnectionLine(): This should never be reached. Direction not handled.");
}

glm::ivec2 CombineLineCoordinates(const glm::ivec2& line, const glm::uvec2& coords)
{
	glm::ivec2 combined;

	if (line.x == -1)
	{
		combined.x = coords.x;
		combined.y = line.y;
	}
	else if (line.y == -1)
	{
		combined.x = line.x;
		combined.y = coords.y;
	}

	return combined;
}

void ConnectGrids2(entt::registry& registry, entt::entity lhs, moveDirection_t lhsConnectDir, entt::entity rhs, moveDirection_t rhsConnectDir)
{
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(0, 19)), Components::Coordinate(rhs, glm::uvec2(0, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(1, 19)), Components::Coordinate(rhs, glm::uvec2(1, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(2, 19)), Components::Coordinate(rhs, glm::uvec2(2, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(3, 19)), Components::Coordinate(rhs, glm::uvec2(3, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(4, 19)), Components::Coordinate(rhs, glm::uvec2(4, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(5, 19)), Components::Coordinate(rhs, glm::uvec2(5, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(6, 19)), Components::Coordinate(rhs, glm::uvec2(6, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(7, 19)), Components::Coordinate(rhs, glm::uvec2(7, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(8, 19)), Components::Coordinate(rhs, glm::uvec2(8, 0)), lhsConnectDir, rhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(lhs, glm::uvec2(9, 19)), Components::Coordinate(rhs, glm::uvec2(9, 0)), lhsConnectDir, rhsConnectDir);

	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(0, 0)), Components::Coordinate(lhs, glm::uvec2(0, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(1, 0)), Components::Coordinate(lhs, glm::uvec2(1, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(2, 0)), Components::Coordinate(lhs, glm::uvec2(2, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(3, 0)), Components::Coordinate(lhs, glm::uvec2(3, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(4, 0)), Components::Coordinate(lhs, glm::uvec2(4, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(5, 0)), Components::Coordinate(lhs, glm::uvec2(5, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(6, 0)), Components::Coordinate(lhs, glm::uvec2(6, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(7, 0)), Components::Coordinate(lhs, glm::uvec2(7, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(8, 0)), Components::Coordinate(lhs, glm::uvec2(8, 19)), rhsConnectDir, lhsConnectDir);
	LinkCoordinates(registry, Components::Coordinate(rhs, glm::uvec2(9, 0)), Components::Coordinate(lhs, glm::uvec2(9, 19)), rhsConnectDir, lhsConnectDir);

	
}

void ConnectGrids(entt::registry& registry, entt::entity lhs, moveDirection_t lhsConnectDir, entt::entity rhs, moveDirection_t rhsConnectDir)
{
	// LinkCoordinates(registry, Components::Coordinate(matrix, glm::uvec2(0, 19)), Components::Coordinate(northBuffer, glm::uvec2(0, 0)), moveDirection_t::NORTH, moveDirection_t::SOUTH);
	// LinkCoordinates(registry, Components::Coordinate(northBuffer, glm::uvec2(0, 0)), Components::Coordinate(matrix, glm::uvec2(0, 19)), moveDirection_t::SOUTH, moveDirection_t::NORTH);
	auto& lhsContainer = registry.get<Components::Container2>(lhs);
	auto& rhsContainer = registry.get<Components::Container2>(rhs);

	auto lhsDimensions = lhsContainer.GetGridDimensions();
	auto rhsDimensions = rhsContainer.GetGridDimensions();

	if (lhsDimensions != rhsDimensions)
		throw std::runtime_error("Grid dimensions must be identical to be connected!");

	glm::ivec2 lhsLine = GetConnectionLine(lhsDimensions, lhsConnectDir);
	glm::ivec2 rhsLine = GetConnectionLine(rhsDimensions, rhsConnectDir);

	auto lhsCellView = registry.view<Components::Cell, Components::Coordinate>();
	auto rhsCellView = registry.view<Components::Cell, Components::Coordinate>();
	for (auto lhsCellEnt : lhsCellView)
	{
		auto& lhsCell = lhsCellView.get<Components::Cell>(lhsCellEnt);
		if (lhs != lhsCell.GetParent())
			continue;

		for (auto rhsCellEnt : rhsCellView)
		{
			if (lhsCellEnt == rhsCellEnt)
				continue;

			auto& rhsCell = rhsCellView.get<Components::Cell>(rhsCellEnt);
			if (rhs != rhsCell.GetParent())
				continue;

			auto& lhsCellCoord = lhsCellView.get<Components::Coordinate>(lhsCellEnt);
			auto& rhsCellCoord = rhsCellView.get<Components::Coordinate>(rhsCellEnt);

			//if (lhsLine.x > -1 && lhsCellCoord.Get().x == rhsCellCoord.Get().x)


			glm::uvec2 lhsCombined = CombineLineCoordinates(lhsLine, lhsCellCoord.Get());
			glm::uvec2 rhsCombined = CombineLineCoordinates(rhsLine, rhsCellCoord.Get());

			int q = 0;
			q++;
			// lhs 0,-1 / 1,-1
			// rhs 2,-1
			//if(lhsLine.x == lhsCellCoord.Get().x && lhsLine.y == lhsCellCoord.Get().y)

			//if(lhsLine.x > -1 && lhsLine.x == lhsCellCoord.Get().x && rhsCellCoord.Get().x == 
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

	const auto playArea = registry.create();
	registry.emplace<Components::Renderable>(playArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));//"./data/quads/block.obj"));
	//registry.emplace<Components::Position>(playArea, glm::vec3(0.0f, 0.0f, 0.0f));
	registry.emplace<Components::Scale>(playArea, glm::vec2(cellWidth * 10, cellHeight * 20)); // celldimensions * gridwidth or height
	registry.emplace<Components::Position>(playArea, glm::vec2(displayData.x / 2, displayData.y / 2));
	//registry.emplace<Components::Scale>(playArea);
	//registry.emplace<Components::Container2>(playArea, glm::uvec2(10, 20), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(playArea, GetTagFromContainerType(containerType_t::PLAY_AREA));

	const auto matrix = registry.create();
	//registry.emplace<Components::Renderable>(matrix, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	registry.emplace<Components::Scale>(matrix, glm::uvec2(cellWidth * 10, cellHeight * 20));
	registry.emplace<Components::Position>(matrix);
	registry.emplace<Components::DerivePositionFromParent>(matrix, playArea);
	registry.emplace<Components::Container2>(matrix, glm::uvec2(10, 20), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));

	const auto bagArea = registry.create();
	registry.emplace<Components::Renderable>(bagArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	registry.emplace<Components::Scale>(bagArea, glm::vec2(25*4, 25*16));
	registry.emplace<Components::Position>(bagArea, glm::vec2(displayData.x - displayData.x / 8, displayData.y / 2));
	registry.emplace<Components::Container2>(bagArea, glm::uvec2(4, 16), glm::vec2(25, 25));
	registry.emplace<Components::Tag>(bagArea, GetTagFromContainerType(containerType_t::BAG_AREA));

	BuildGrid(registry, matrix);
	BuildGrid(registry, bagArea);

	PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Matrix Edge 1", Components::Coordinate(matrix, glm::uvec2(0, 0)));

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
	//glEnable(GL_DEPTH_TEST);

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

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);

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