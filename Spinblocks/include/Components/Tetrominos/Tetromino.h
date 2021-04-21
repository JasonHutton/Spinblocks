#pragma once

#include "Globals.h"
#include <entt/entity/registry.hpp>
#include "Components/Component.h"
#include "Components/Obstructable.h"
#include "Components/Moveable.h"
#include "Components/Block.h"
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <tuple>
#include "glm/vec2.hpp"

enum class rotationDirection_t;
enum class moveDirection_t;
enum class tetrominoType_t;

namespace Components
{
	class Tetromino : public Component
	{
	protected:
		moveDirection_t m_currentOrientation{ moveDirection_t::NORTH };
		moveDirection_t m_desiredOrientation{ moveDirection_t::NORTH };
		tetrominoType_t m_tetrominoType;

		std::vector<entt::entity> m_blocks;
		std::vector<glm::vec2> m_rotationPoints;

		/*
		Map's Key is in a Tuple:
		Key0: rotationPointIndex (int)
		Key1: currentOrientation (moveDirection_t)
		Key2: rotationDirection (rotationDirection_t) Often NONE
		Map's Value is a Pair:
		Pair.first: blockPattern (vector<glm::vec2>)
		Pair.second: Key Tuple. (Identical types and orders to those composing the Key.)
		When retrieving this, if Pair.first is empty, query it again, using Pair.second as a key.
		*/
		std::map<std::tuple<int, moveDirection_t, rotationDirection_t>, std::pair<std::vector<glm::vec2>, std::tuple<int, moveDirection_t, rotationDirection_t>>> m_blockPatterns;
	public:
		void AddBlockPattern(int rotationPointIndex, moveDirection_t orientation, rotationDirection_t rotationDirection, std::vector<glm::vec2> blockPattern)
		{
			m_blockPatterns.emplace(std::make_tuple(rotationPointIndex, orientation, rotationDirection), std::make_pair(blockPattern, std::tuple<int, moveDirection_t, rotationDirection_t>()));
		}

		void AddBlockPattern(int rotationPointIndex, moveDirection_t orientation, rotationDirection_t rotationDirection, std::tuple<int, moveDirection_t, rotationDirection_t> alternativeIndex)
		{
			m_blockPatterns.emplace(std::make_tuple(rotationPointIndex, orientation, rotationDirection), std::make_pair(std::vector<glm::vec2>(), alternativeIndex));
		}

	public:
		// Width of the defining pattern of the Tetromino
		virtual const int GetPatternWidth() = 0;

		// Height of the defining pattern of the Tetromino
		virtual const int GetPatternHeight() = 0;

	public:
		virtual const std::string GetBlockModelPath() = 0;

		// virtual method to determine pattern based on table and rotation adn etc
	protected:
		/*
		* Virtual method to define the pattern of blocks that compose this Tetromino
		* This pattern should always be defined from a North facing.
		* Origin should be considered to be lower-left.
		*/
		virtual void DefineBlockPattern() = 0;
		/*
		* Rotation Points use the same origin as block patterns.
		* The first rotation point is the starting center of the Tetromino
		*/
		virtual void DefineRotationPoints() = 0;

	public:
		Tetromino(const tetrominoType_t& tetrominoType, const moveDirection_t& orientation) : m_currentOrientation(orientation), m_desiredOrientation(orientation), m_tetrominoType(tetrominoType)
		{
		}

		const moveDirection_t& GetCurrentOrientation() const
		{
			return m_currentOrientation;
		}

		const moveDirection_t& GetDesiredOrientation() const
		{
			return m_desiredOrientation;
		}

		void SetCurrentOrientation(const moveDirection_t& moveDirection)
		{
			m_currentOrientation = moveDirection;
		}

		void SetDesiredOrientation(const moveDirection_t& moveDirection)
		{
			m_desiredOrientation = moveDirection;
		}

		const tetrominoType_t& GetType() const
		{
			return m_tetrominoType;
		}

		bool GetAreAllBlocksObstructed(entt::registry& registry) const
		{
			for (int i = 0; i < 4; i++)
			{
				if (registry.all_of<Components::Obstructable>(GetBlock(i)))
				{
					const auto& obstructable = registry.get<Components::Obstructable>(GetBlock(i));
					if (!obstructable.GetIsObstructed())
					{
						return false;
					}
				}
			}

			return true;
		}

		void SetAllBlocksObstructed(entt::registry& registry, bool isObstructed)
		{
			for (int i = 0; i < 4; i++)
			{
				if (registry.all_of<Components::Obstructable>(GetBlock(i)))
				{
					auto& obstructable = registry.get<Components::Obstructable>(GetBlock(i));
					obstructable.SetIsObstructed(isObstructed);
				}
			}
		}

		double GetAllBlocksLastObstructedTime(entt::registry& registry) const
		{
			double lastObstructedTime = 0.0;
			for (int i = 0; i < 4; i++)
			{
				if (registry.all_of<Components::Obstructable>(GetBlock(i)))
				{
					const auto& obstructable = registry.get<Components::Obstructable>(GetBlock(i));
					if (obstructable.GetLastObstructedTime() > lastObstructedTime)
						lastObstructedTime = obstructable.GetLastObstructedTime();
				}
			}

			return lastObstructedTime;
		}

		void SetAllBlocksLastObstructedTime(entt::registry& registry, double lastObstructedTime)
		{
			for (int i = 0; i < 4; i++)
			{
				if (registry.all_of<Components::Obstructable>(GetBlock(i)))
				{
					auto& obstructable = registry.get<Components::Obstructable>(GetBlock(i));
					obstructable.SetLastObstructedTime(lastObstructedTime);
				}
			}
		}

		void SetAllBlocksMovementState(entt::registry& registry, Components::movementStates_t movementState)
		{
			for (int i = 0; i < 4; i++)
			{
				if (registry.all_of<Components::Moveable>(GetBlock(i)))
				{
					auto& moveable = registry.get<Components::Moveable>(GetBlock(i));
					moveable.SetMovementState(movementState);
				}
			}
		}

		void SetAllBlocksMovementState(entt::registry& registry, Components::movementStates_t movementState, std::vector<BlockLockData>& blockLockData)
		{
			for (int i = 0; i < 4; i++)
			{
				if (registry.all_of<Components::Moveable>(GetBlock(i)))
				{
					auto& moveable = registry.get<Components::Moveable>(GetBlock(i));
					moveable.SetMovementState(movementState);
					blockLockData.push_back(moveable.GetCurrentCoordinate());
				}
			}
		}

		bool GetAreAllBlocksLocked(entt::registry& registry) const
		{
			for (int i = 0; i < 4; i++)
			{
				if (registry.all_of<Components::Moveable>(GetBlock(i)))
				{
					const auto& moveable = registry.get<Components::Moveable>(GetBlock(i));
					if (moveable.GetMovementState() != Components::movementStates_t::LOCKED)
						return false;
				}
			}

			return true;
		}

		void AddBlock(entt::entity block)
		{
			m_blocks.push_back(block);
		}

		const entt::entity& GetBlock(int blockIndex) const
		{
			return m_blocks[blockIndex];
		}

		// void AddBlock2(int rotationPointIndex, moveDirection_t orientation, rotationDirection_t rotationDirection, std::vector<glm::vec2> blockPattern)
		glm::vec2 GetBlockOffsetCoordinates(moveDirection_t orientation, int blockIndex, int rotationPointIndex = 0, rotationDirection_t rotationDirection = rotationDirection_t::NONE)
		{
			auto& patternPair = m_blockPatterns[std::make_tuple(rotationPointIndex, orientation, rotationDirection)];
			if (patternPair.first.empty())
			{
				patternPair = m_blockPatterns[std::make_tuple(std::get<0>(patternPair.second), std::get<1>(patternPair.second), std::get<2>(patternPair.second))];
			}

			return patternPair.first[blockIndex] - m_rotationPoints[rotationPointIndex];
		}

		const moveDirection_t GetNewOrientation(const rotationDirection_t& rotationDirection, const moveDirection_t& currentOrientation) const
		{
			if (rotationDirection == rotationDirection_t::CLOCKWISE)
			{
				switch (currentOrientation)
				{
				case moveDirection_t::NORTH:
					return moveDirection_t::EAST;
				case moveDirection_t::EAST:
					return moveDirection_t::SOUTH;
				case moveDirection_t::SOUTH:
					return moveDirection_t::WEST;
				case moveDirection_t::WEST:
					return moveDirection_t::NORTH;
				default:
					assert(false);
				}
			}
			else if (rotationDirection == rotationDirection_t::COUNTERCLOCKWISE)
			{
				switch (currentOrientation)
				{
				case moveDirection_t::NORTH:
					return moveDirection_t::WEST;
				case moveDirection_t::WEST:
					return moveDirection_t::SOUTH;
				case moveDirection_t::SOUTH:
					return moveDirection_t::EAST;
				case moveDirection_t::EAST:
					return moveDirection_t::NORTH;
				default:
					assert(false);
				}
			}

			assert(false);
		}
	};
}