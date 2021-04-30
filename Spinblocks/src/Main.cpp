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

#include "Input/InputHandler.h"
#include "Input/GameInput.h"

#include "GameState.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

// GL 3.0 + GLSL 130 // From ImGUI
const char* glsl_version = "#version 130";

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
			registry.emplace<Components::Renderable>(marker, Components::renderLayer_t::RL_MARKER_UNDER, Model("./data/block/green.obj"));
		}
	}
}

entt::entity PlaceBagMarker(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& markerCoordinate, const Components::renderLayer_t& layer = Components::renderLayer_t::RL_MARKER_UNDER)
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
			registry.emplace<Components::QueueNode>(marker, marker); // Don't attempt to link nodes at first
			registry.emplace<Components::Coordinate>(marker, markerCoordinate.GetParent(), markerCoordinate.Get());
			registry.emplace<Components::Position>(marker);
			registry.emplace<Components::DerivePositionFromCoordinates>(marker);
			registry.emplace<Components::Scale>(marker, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(marker, layer, Model("./data/block/green.obj"));
			registry.emplace<Components::Orientation>(marker);
			registry.emplace<Components::ReferenceEntity>(marker, entity);

			return marker;
		}
	}

	return entt::null;
}

void PlaceMarker(entt::registry& registry, const std::string& containerTag, const std::string& markerTag, const Components::Coordinate& markerCoordinate, const Components::renderLayer_t& layer = Components::renderLayer_t::RL_MARKER_UNDER, const entt::entity followedEnt = entt::null)
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
			registry.emplace<Components::Renderable>(marker, layer, Model("./data/block/red.obj"));
			registry.emplace<Components::Tag>(marker, markerTag);
			if (followedEnt != entt::null)
			{
				registry.emplace<Components::Follower>(marker, followedEnt);
			}
			registry.emplace<Components::Orientation>(marker);
			registry.emplace<Components::ReferenceEntity>(marker, entity);
		}
	}
}

void PlaceSpawnMarker(entt::registry& registry, const std::string& containerTag, const Components::Coordinate& markerCoordinate, const spawnType_t& spawnType, const moveDirection_t& activeDirection, const Components::renderLayer_t& layer = Components::renderLayer_t::RL_MARKER_UNDER)
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
			registry.emplace<Components::SpawnMarker>(marker, entity, spawnType);
			registry.emplace<Components::Coordinate>(marker, markerCoordinate.GetParent(), markerCoordinate.Get());
			registry.emplace<Components::Position>(marker);
			registry.emplace<Components::DerivePositionFromCoordinates>(marker);
			registry.emplace<Components::Scale>(marker, container2.GetCellDimensions3());
			//registry.emplace<Components::Renderable>(marker, layer, Model("./data/block/lightblue.obj"));
			registry.emplace<Components::Orientation>(marker);
			registry.emplace<Components::ReferenceEntity>(marker, entity);
			registry.emplace<Components::DirectionallyActive>(marker, activeDirection);
		}
	}
}

void PlaceWall(entt::registry& registry, const Components::Coordinate& coordinate, const bool& directional, const std::vector<moveDirection_t> directions)
{
	auto containerView = registry.view<Components::Container2>();
	for (auto containerEnt : containerView)
	{
		auto& container2 = containerView.get<Components::Container2>(containerEnt);

		if (container2.IsEnabled())
		{
			if (containerEnt != coordinate.GetParent())
				continue;

			entt::entity cellEnt = GetCellAtCoordinates2(registry, coordinate);

			if (cellEnt == entt::null)
				continue;

			const auto wall = registry.create();
			registry.emplace<Components::Wall>(wall);
			registry.emplace<Components::Coordinate>(wall, coordinate.GetParent(), coordinate.Get());
			registry.emplace<Components::Position>(wall);
			registry.emplace<Components::DerivePositionFromCoordinates>(wall);
			registry.emplace<Components::Scale>(wall, container2.GetCellDimensions3());
			registry.emplace<Components::Renderable>(wall, Components::renderLayer_t::RL_MARKER_OVER, Model("./data/block/grey.obj"));
			registry.emplace<Components::Obstructs>(wall);
			registry.emplace<Components::Orientation>(wall);
			registry.emplace<Components::ReferenceEntity>(wall, coordinate.GetParent());
			if (directional)
			{
				registry.emplace<Components::DirectionallyActive>(wall, directions);
			}
		}
	}
}

void MoveTetromino(entt::registry& registry, const movePiece_t& movePiece)
{
	/*
	auto controllableView = registry.view<Components::Controllable, Components::Moveable>();
	for (auto entity : controllableView)
	{
		auto& controllable = controllableView.get<Components::Controllable>(entity);
		auto& moveable = controllableView.get<Components::Moveable>(entity);

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
	}*/
}

void RotatePiece(entt::registry& registry, const rotatePiece_t& rotatePiece)
{
	entt::entity tetrominoEntity = GetActiveControllable(registry);
	if (tetrominoEntity == entt::null)
		return;

	auto* tetromino = GetTetrominoFromEntity(registry, tetrominoEntity);
	auto tetrominoCoord = registry.get<Components::Coordinate>(tetrominoEntity);

	moveDirection_t desiredOrientation;

	try
	{
		switch (rotatePiece)
		{
		case rotatePiece_t::ROTATE_CLOCKWISE:
			desiredOrientation = tetromino->GetNewOrientation(rotationDirection_t::CLOCKWISE, tetromino->GetCurrentOrientation());
			break;
		case rotatePiece_t::ROTATE_COUNTERCLOCKWISE:
			desiredOrientation = tetromino->GetNewOrientation(rotationDirection_t::COUNTERCLOCKWISE, tetromino->GetCurrentOrientation());
			break;
		default:
			desiredOrientation = tetromino->GetCurrentOrientation();
			break;
		}
	}
	catch (std::runtime_error ex)
	{
		cerr << ex.what() << endl;
	}

	bool atLeastOneBlockObstructed = false;
	for (int i = 0; i < 4; i++)
	{
		auto& blockEnt = tetromino->GetBlock(i);
		auto& blockCoord = registry.get<Components::Coordinate>(blockEnt);

		auto offsetCoordinate = Components::Coordinate(blockCoord.GetParent(),
			(glm::vec2)blockCoord.Get() + 
			tetromino->GetBlockOffsetCoordinates(tetromino->GetCurrentOrientation(), i, 0, rotatePiece == rotatePiece_t::ROTATE_CLOCKWISE ? rotationDirection_t::CLOCKWISE : rotationDirection_t::COUNTERCLOCKWISE));

		auto cellEnt = GetCellAtCoordinates2(registry, offsetCoordinate);
		if (!CanOccupyCell(registry, blockEnt, cellEnt))
			atLeastOneBlockObstructed = true;
	}

	if (!atLeastOneBlockObstructed)
	{ // Quick and dirty, rather than handling through a system. Refactor later. FIXME TODO
		tetromino->SetDesiredOrientation(desiredOrientation);
		tetromino->SetCurrentOrientation(tetromino->GetDesiredOrientation());
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

							auto& playAreaRefEnt = registry.get<Components::ReferenceEntity>(moveable.GetCurrentCoordinate().GetParent());
							auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaRefEnt.Get());

							try
							{
								switch (movePiece)
								{
								case movePiece_t::MOVE_LEFT:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, playAreaDirection.GetCurrentLeftDirection(), 1)));
									break;
								case movePiece_t::MOVE_RIGHT:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, playAreaDirection.GetCurrentRightDirection(), 1)));
									break;
								case movePiece_t::MOVE_UP:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, playAreaDirection.GetCurrentUpDirection(), 1)));
									moveable.SetMovementState(Components::movementStates_t::DEBUG_MOVE_UP);
									break;
								case movePiece_t::SOFT_DROP:
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, playAreaDirection.GetCurrentDownDirection(), 1)));
									moveable.SetMovementState(Components::movementStates_t::SOFT_DROP);
									break;
								case movePiece_t::HARD_DROP:
								{
									moveable.SetDesiredCoordinate(GetCoordinateOfEntity(registry, MoveBlockInDirection(registry, entity1, playAreaDirection.GetCurrentDownDirection(), PlayAreaHeight + (BufferAreaDepth * 2)))); // PlayAreaHeight is a const of 20. This is fine to be excessive with when the height is lower.
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
#ifdef _DEBUG
			case KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_1:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				//SpawnBlock(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX)), glm::uvec2(0, 19)));
				//SpawnBlock(registry, GetTagFromContainerType(containerType_t::BAG_AREA), Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::BAG_AREA)), glm::uvec2(1, 15)));

				auto controllableView = registry.view<Components::Controllable>();
				for (auto controllable : controllableView)
				{
					registry.remove_if_exists<Components::Controllable>(controllable);
				}

				auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX),
					Components::Coordinate(FindContainerEntityByTag(registry,
						GetTagFromContainerType(containerType_t::MATRIX)),
						GetTetrominoSpawnCoordinates(registry, GetTagFromContainerType(containerType_t::MATRIX), tetrominoType_t::O)),
					tetrominoType_t::O);
				//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "O Tetromino Marker", Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX)), GetTetrominoSpawnCoordinates(tetrominoType_t::O)), Components::renderLayer_t::RL_MARKER_OVER, tet);

				if (registry.all_of<Components::Moveable>(tet))
				{
					auto& moveable = registry.get<Components::Moveable>(tet);
					moveable.SetMovementState(Components::movementStates_t::FALL);
				}

				if (IsAnyBlockInTetrominoObstructed(registry, tet))
				{
					cout << "Obstructed on spawn! Game Over!" << endl;

					auto controllableView = registry.view<Components::Controllable>();
					for (auto controllable : controllableView)
					{
						registry.remove_if_exists<Components::Controllable>(controllable);
					}
					GameState::SetState(gameState_t::GAME_OVER);
				}

				break;
			}
			case KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_2:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				auto controllableView = registry.view<Components::Controllable>();
				for (auto controllable : controllableView)
				{
					registry.remove_if_exists<Components::Controllable>(controllable);
				}

				auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX), 
					Components::Coordinate(FindContainerEntityByTag(registry, 
						GetTagFromContainerType(containerType_t::MATRIX)), 
						GetTetrominoSpawnCoordinates(registry, GetTagFromContainerType(containerType_t::MATRIX), tetrominoType_t::I)),
					tetrominoType_t::I);
				//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "I Tetromino Marker", Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX)), GetTetrominoSpawnCoordinates(tetrominoType_t::I)), Components::renderLayer_t::RL_MARKER_OVER, tet);

				if (registry.all_of<Components::Moveable>(tet))
				{
					auto& moveable = registry.get<Components::Moveable>(tet);
					moveable.SetMovementState(Components::movementStates_t::FALL);
				}

				if (IsAnyBlockInTetrominoObstructed(registry, tet))
				{
					cout << "Obstructed on spawn! Game Over!" << endl;

					auto controllableView = registry.view<Components::Controllable>();
					for (auto controllable : controllableView)
					{
						registry.remove_if_exists<Components::Controllable>(controllable);
					}
					GameState::SetState(gameState_t::GAME_OVER);
				}

				break;
			}
			case KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_3:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				auto controllableView = registry.view<Components::Controllable>();
				for (auto controllable : controllableView)
				{
					registry.remove_if_exists<Components::Controllable>(controllable);
				}

				auto tet = SpawnTetromino(registry, GetTagFromContainerType(containerType_t::MATRIX), 
					Components::Coordinate(FindContainerEntityByTag(registry, 
						GetTagFromContainerType(containerType_t::MATRIX)), 
						GetTetrominoSpawnCoordinates(registry, GetTagFromContainerType(containerType_t::MATRIX), tetrominoType_t::T)),
					tetrominoType_t::T);
				//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "O Tetromino Marker", Components::Coordinate(FindContainerEntityByTag(registry, GetTagFromContainerType(containerType_t::MATRIX)), GetTetrominoSpawnCoordinates(tetrominoType_t::O)), Components::renderLayer_t::RL_MARKER_OVER, tet);

				if (registry.all_of<Components::Moveable>(tet))
				{
					auto& moveable = registry.get<Components::Moveable>(tet);
					moveable.SetMovementState(Components::movementStates_t::FALL);
				}

				if(IsAnyBlockInTetrominoObstructed(registry, tet))
				{
					cout << "Obstructed on spawn! Game Over!" << endl;

					auto controllableView = registry.view<Components::Controllable>();
					for (auto controllable : controllableView)
					{
						registry.remove_if_exists<Components::Controllable>(controllable);
					}
					GameState::SetState(gameState_t::GAME_OVER);
				}
				

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
			case KeyInput::usercmdButton_t::UB_DEBUG_ROTATE_PLAY_AREA_COUNTERCLOCKWISE:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				RotatePlayArea(registry, rotationDirection_t::COUNTERCLOCKWISE);

				break;
			}
			case KeyInput::usercmdButton_t::UB_DEBUG_ROTATE_PLAY_AREA_CLOCKWISE:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				RotatePlayArea(registry, rotationDirection_t::CLOCKWISE);

				break;
			}
			case KeyInput::usercmdButton_t::UB_DEBUG_PROJECT_DOWN:
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
						if (IsEntityTetromino(registry, entity))
						{
							auto cell = FindLowestCell(registry, entity);
							int q = 0;
							q++;
						}
					}
				}
				break;
			}
#endif
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
			case KeyInput::usercmdButton_t::UB_ROTATE_COUNTERCLOCKWISE:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				RotatePiece(registry, rotatePiece_t::ROTATE_COUNTERCLOCKWISE);
				break;
			}
			case KeyInput::usercmdButton_t::UB_ROTATE_CLOCKWISE:
			{
				if (keyState.second.prevKeyDown == true)
					break;

				RotatePiece(registry, rotatePiece_t::ROTATE_CLOCKWISE);
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
	auto cardinalDirectionView = registry.view<Components::CardinalDirection, Components::Orientation>();
	for (auto entity : cardinalDirectionView)
	{
		auto& cardinalDirection = cardinalDirectionView.get<Components::CardinalDirection>(entity);
		auto& orientation = cardinalDirectionView.get<Components::Orientation>(entity);

		if (cardinalDirection.IsEnabled() && orientation.IsEnabled())
		{
			if (cardinalDirection.GetCurrentOrientation() != cardinalDirection.GetDesiredOrientation())
			{

			}
		}
	}
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

	auto blockLockData = std::vector<BlockLockData>();

	Systems::GenerationSystem(registry, currentFrameTime);
	Systems::FallingSystem(registry, currentFrameTime);
	Systems::MovementSystem(registry, currentFrameTime);
	Systems::StateChangeSystem(registry, currentFrameTime, blockLockData);


	const auto& playAreaEnt = FindEntityByTag(registry, GetTagFromContainerType(containerType_t::PLAY_AREA));
	if (playAreaEnt == entt::null)
		throw std::runtime_error("Play Area entity is null!");
	auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaEnt);

	int lineLength = 0;
	if(playAreaDirection.GetCurrentOrientation() == moveDirection_t::NORTH || playAreaDirection.GetCurrentOrientation() == moveDirection_t::SOUTH)
	{
		lineLength = PlayAreaWidth;
	}
	else
	{
		lineLength = PlayAreaHeight;
	}

	int linesMatched = 0;

	linesMatched = Systems::PatternSystem(registry, lineLength, currentFrameTime);
	Systems::EliminateSystem(registry, currentFrameTime);

	rotationDirection_t shouldBoardRotate = ChooseBoardRotationDirection(registry, blockLockData, playAreaDirection.GetCurrentOrientation(), linesMatched);

	if (Systems::BoardRotateSystem(registry, currentFrameTime, shouldBoardRotate) != rotationDirection_t::NONE)
	{ // We did a rotation.
		// Handle any elimination that should have happened.
		// This ensures any falling realignment after an elimination occurs.
		Systems::MovementSystem(registry, currentFrameTime);
		Systems::StateChangeSystem(registry, currentFrameTime, blockLockData);
		// Now detach, after the falling realignment.
		Systems::DetachSystem(registry, currentFrameTime);
	}

	Systems::CompletionSystem(registry, currentFrameTime, linesMatched);


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
	auto orientationFromParentView = registry.view<Components::DeriveOrientationFromParent, Components::Orientation>();
	for (auto entity : orientationFromParentView)
	{
		auto& deriveOrientationFromParent = orientationFromParentView.get<Components::DeriveOrientationFromParent>(entity);
		auto& orientation = orientationFromParentView.get<Components::Orientation>(entity);

		if (deriveOrientationFromParent.IsEnabled() && orientation.IsEnabled())
		{
			if(!registry.all_of<Components::Orientation>(deriveOrientationFromParent.Get()))
				continue;

			Components::Orientation parentOrientation = registry.get<Components::Orientation>(deriveOrientationFromParent.Get());

			orientation.Set(parentOrientation.Get() + deriveOrientationFromParent.GetOffset());
			orientation.SetAxis(parentOrientation.GetAxis() + deriveOrientationFromParent.GetAxisOffset());
		}
	}

	/*auto scaleView = registry.view<Components::ScaleToCellDimensions, Components::Scale, Components::Cell>();
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
	}*/

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
			Components::Position parentPosition = registry.get<Components::Position>(deriveCoordinatesFrom); // this is a 0 vector in Matrix:Grid:0-0, 700,300 in BagArea:Grid:0-0 // Also 0 vector with blocks.
			Components::Container2 container2 = registry.get<Components::Container2>(deriveCoordinatesFrom);

			// Review GetCellPosition3() later. What should it be in reference to? Parent entity? Matrix? Parent coordinates? FIXME TODO
			//position.Set(container2.GetCellPosition3(parentPosition.Get(), coordinates.Get()) + derivePositionFromCoordinates.GetOffset());
			position.Set(container2.GetCellPosition3(glm::vec3(0.0, 0.0, 0.0), coordinates.Get()) + derivePositionFromCoordinates.GetOffset());
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

	auto renderView = registry.view<Components::Renderable, Components::Position, Components::Orientation, Components::Scale>();
	for (int i = Components::renderLayer_t::RL_MIN+1; i < Components::renderLayer_t::RL_MAX; i++)
	{
		for (auto entity : renderView)
		{
			auto& render = renderView.get<Components::Renderable>(entity);
			
			if (render.GetLayer() != i)
				continue;

			// We grab these after checking the renderlayer as a simplistic optimization. May not even be relevant, hasn't been benchmarked..
			auto& position = renderView.get<Components::Position>(entity);
			auto& orientation = renderView.get<Components::Orientation>(entity);
			auto& scale = renderView.get<Components::Scale>(entity);

			if (render.IsEnabled() && position.IsEnabled() && orientation.IsEnabled() && scale.IsEnabled())
			{
				bool inheritScaling = false;
				if (registry.all_of<Components::InheritScalingFromParent>(entity))
				{
					auto& inheritScalingFromParent = registry.get<Components::InheritScalingFromParent>(entity);
					inheritScaling = inheritScalingFromParent.Get();
				}
				
				shader->setMat4("model", GetModelMatrixOfEntity(registry, entity, inheritScaling));
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

void InitGame(entt::registry& registry)
{
	GameInput::setVerticalAxis(0);
	GameInput::setHorizontalAxis(0);

	GameScore = 0;
	LevelGoal = StartLevelGoal;
	LinesClearedTotal = 0;

	GameLevel = StartGameLevel;
	FallSpeed = CalculateFallSpeed(GameLevel);

	const auto camera = registry.create();
	registry.emplace<Components::OrthographicCamera>(camera, glm::vec3(0.0f, 0.0f, 3.0f));
	//registry.emplace<Components::PerspectiveCamera>(camera, glm::vec3(0.0f, 0.0f, 3.0f));

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
	registry.emplace<Components::Container2>(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth * 2), PlayAreaHeight + (BufferAreaDepth * 2)), glm::uvec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(matrix, GetTagFromContainerType(containerType_t::MATRIX));
	registry.emplace<Components::Orientation>(matrix);
	registry.emplace<Components::ReferenceEntity>(matrix, playArea);
	//registry.emplace<Components::DeriveOrientationFromParent>(matrix, playArea);
	registry.emplace<Components::InheritScalingFromParent>(matrix, false);
	
	const auto bagArea = registry.create();
	registry.emplace<Components::Renderable>(bagArea, Components::renderLayer_t::RL_CONTAINER, Model("./data/block/block.obj"));
	registry.emplace<Components::Scale>(bagArea, glm::vec2(25 * 4, 25 * 16));
	registry.emplace<Components::Position>(bagArea, glm::vec2(displayData.x - displayData.x / 11, displayData.y / 2));
	registry.emplace<Components::Container2>(bagArea, glm::uvec2(4, 16), glm::vec2(cellWidth, cellHeight));
	registry.emplace<Components::Tag>(bagArea, GetTagFromContainerType(containerType_t::BAG_AREA));
	registry.emplace<Components::Orientation>(bagArea);
	//registry.emplace<Components::ReferenceEntity>(bagArea, playArea);
	registry.emplace<Components::InheritScalingFromParent>(bagArea, false);
	registry.emplace<Components::Bag>(bagArea);
	registry.emplace<Components::NodeOrder>(bagArea);

	BuildGrid(registry, matrix);
	BuildGrid(registry, bagArea);
	
	/*
	// North
	for (int i = BufferAreaDepth; i < PlayAreaWidth + BufferAreaDepth; i++)
	{
		PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Matrix Edge 1", Components::Coordinate(matrix, glm::uvec2(i, PlayAreaHeight + (BufferAreaDepth - 1))));
	}*/

	for (int i = BufferAreaDepth-1; i < PlayAreaWidth + BufferAreaDepth+1; i++)
	{
		//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Border 1", Components::Coordinate(matrix, glm::uvec2(i, PlayAreaHeight + (BufferAreaDepth - 1) + 1)));
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(i, PlayAreaHeight + (BufferAreaDepth - 1) + 1)), true, { moveDirection_t::SOUTH, moveDirection_t::EAST, moveDirection_t::WEST });
	}

	/*
	// South
	for (int i = BufferAreaDepth; i < PlayAreaWidth + BufferAreaDepth; i++)
	{
		PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Matrix Edge 2", Components::Coordinate(matrix, glm::uvec2(i, 0 + BufferAreaDepth)));
	}*/
	
	for (int i = BufferAreaDepth - 1; i < PlayAreaWidth + BufferAreaDepth + 1; i++)
	{
		//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Border 2", Components::Coordinate(matrix, glm::uvec2(i, 0 + (BufferAreaDepth - 1))));
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(i, 0 + (BufferAreaDepth - 1))), true, { moveDirection_t::NORTH, moveDirection_t::EAST, moveDirection_t::WEST });
	}
	
	/*// West
	for (int i = BufferAreaDepth; i < PlayAreaHeight + BufferAreaDepth; i++)
	{
		PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Matrix Edge 3", Components::Coordinate(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth - 1), i)));
	}*/

	for (int i = BufferAreaDepth - 1; i < PlayAreaHeight + BufferAreaDepth + 1; i++)
	{
		//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Border 3", Components::Coordinate(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth - 1) + 1, i)));
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(PlayAreaWidth + (BufferAreaDepth - 1) + 1, i)), true, { moveDirection_t::NORTH, moveDirection_t::SOUTH, moveDirection_t::EAST });
	}
	
	/*
	// East
	for (int i = BufferAreaDepth; i < PlayAreaHeight + BufferAreaDepth; i++)
	{
		PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Matrix Edge 4", Components::Coordinate(matrix, glm::uvec2(0 + BufferAreaDepth, i)));
	}*/

	for (int i = BufferAreaDepth - 1; i < PlayAreaHeight + BufferAreaDepth + 1; i++)
	{
		//PlaceMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), "Border 4", Components::Coordinate(matrix, glm::uvec2(0 + (BufferAreaDepth - 1), i)));
		PlaceWall(registry, Components::Coordinate(matrix, glm::uvec2(0 + (BufferAreaDepth - 1), i)), true, { moveDirection_t::NORTH, moveDirection_t::SOUTH, moveDirection_t::WEST });
	}

	UpdateDirectionalWalls(registry);

	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(4 + BufferAreaDepth, 21 + (BufferAreaDepth - 1))), spawnType_t::ITETROMINO, moveDirection_t::NORTH);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(4 + BufferAreaDepth, 0 + BufferAreaDepth)), spawnType_t::ITETROMINO, moveDirection_t::SOUTH);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0 + (BufferAreaDepth - 2), 10 + BufferAreaDepth)), spawnType_t::ITETROMINO, moveDirection_t::EAST);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(10 + BufferAreaDepth, 10 + BufferAreaDepth)), spawnType_t::ITETROMINO, moveDirection_t::WEST);

	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(4 + BufferAreaDepth, 21 + (BufferAreaDepth - 1))), spawnType_t::OTETROMINO, moveDirection_t::NORTH);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(4 + BufferAreaDepth, 0 + (BufferAreaDepth - 2))), spawnType_t::OTETROMINO, moveDirection_t::SOUTH);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0 + (BufferAreaDepth - 2), 9 + BufferAreaDepth)), spawnType_t::OTETROMINO, moveDirection_t::EAST);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(10 + BufferAreaDepth, 9 + BufferAreaDepth)), spawnType_t::OTETROMINO, moveDirection_t::WEST);

	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(4 + BufferAreaDepth, 21 + (BufferAreaDepth - 1))), spawnType_t::WIDTH3, moveDirection_t::NORTH);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(5 + BufferAreaDepth, 0 + (BufferAreaDepth - 1))), spawnType_t::WIDTH3, moveDirection_t::SOUTH);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(0 + (BufferAreaDepth - 1), 10 + (BufferAreaDepth - 1))), spawnType_t::WIDTH3, moveDirection_t::EAST);
	PlaceSpawnMarker(registry, GetTagFromContainerType(containerType_t::MATRIX), Components::Coordinate(matrix, glm::uvec2(10 + BufferAreaDepth, 10 + BufferAreaDepth)), spawnType_t::WIDTH3, moveDirection_t::WEST);

	// Setup the nodes
	auto& nodeOrder = registry.get<Components::NodeOrder>(bagArea);

	nodeOrder.AddNode(PlaceBagMarker(registry, GetTagFromContainerType(containerType_t::BAG_AREA), Components::Coordinate(bagArea, glm::uvec2(1, 1))));
	nodeOrder.AddNode(PlaceBagMarker(registry, GetTagFromContainerType(containerType_t::BAG_AREA), Components::Coordinate(bagArea, glm::uvec2(1, 5))));
	nodeOrder.AddNode(PlaceBagMarker(registry, GetTagFromContainerType(containerType_t::BAG_AREA), Components::Coordinate(bagArea, glm::uvec2(1, 9))));
	nodeOrder.AddNode(PlaceBagMarker(registry, GetTagFromContainerType(containerType_t::BAG_AREA), Components::Coordinate(bagArea, glm::uvec2(1, 13))));

	LinkNodes(registry, nodeOrder, bagArea, matrix);
}

void TeardownGame(entt::registry& registry)
{
	/*registry.each([&](auto entity) {
		registry.destroy(entity);
		});*/
	registry.clear();
	cachedTagLookup.Clear();
}

void ImGUIInit(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}

void ImGUITeardown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGUIFrameInit()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGUIFrameEnd()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	GameState::SetState(gameState_t::INIT);

	entt::registry registry;

	glfwSwapInterval(1);
	//glEnable(GL_DEPTH_TEST);

	ImGUIInit(window);

	// Do one-time OpenGL things here.
	Shader* shader = RetrieveShader("model", "./data/shaders/1.model_loading.vs", "./data/shaders/1.model_loading.fs");
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GameTime::Initialize(glfwGetTime());

	while (!glfwWindowShouldClose(window))
	{
		double currentFrameTime = glfwGetTime();
		double deltaTime = currentFrameTime - GameTime::lastFrameTime;
		GameTime::lastFrameTime = currentFrameTime;
		GameTime::accumulator += deltaTime;

		if (GameState::GetState() == gameState_t::INIT)
		{
			InitGame(registry);

			GameState::SetState(gameState_t::MENU); // Placeholder.
			GameState::SetState(gameState_t::PLAY);
		}

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);

		if (GameState::GetState() == gameState_t::PLAY)
		{
			processinput(window, registry, currentFrameTime);
		}

		while (GameTime::accumulator >= GameTime::fixedDeltaTime)
		{
			if (GameState::GetState() == gameState_t::PLAY)
			{
				// Update game logic for ECS
				preupdate(registry, currentFrameTime);
				update(registry, currentFrameTime);
				postupdate(registry, currentFrameTime);
			}

			GameTime::accumulator -= GameTime::fixedDeltaTime;
		}
		// Update render objects.
		prerender(registry, GameTime::accumulator / GameTime::fixedDeltaTime);
		render(registry, GameTime::accumulator / GameTime::fixedDeltaTime);
		postrender(registry, GameTime::accumulator / GameTime::fixedDeltaTime);

		ImGUIFrameInit();

		// IMGUI begin
		char buf[500] = "Blah blah blah.\n";
		float f = 0.0f;
		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Save"))
		{
			// Do nothing
		}
		ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		// IMGUI end

		ImGUIFrameEnd();

		if (GameState::GetState() == gameState_t::GAME_OVER)
		{
			TeardownGame(registry);
			GameState::SetState(gameState_t::INIT);
		}

		glfwSwapBuffers(window);
		glfwPollEvents(); // Windows needs to do things with the window too!
	}

	ImGUITeardown();

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