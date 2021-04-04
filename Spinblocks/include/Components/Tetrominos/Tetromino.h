#pragma once

#include "Globals.h"
#include <entt/entity/registry.hpp>
#include "Components/Component.h"
#include "Components/Obstructable.h"
#include "Components/Moveable.h"
#include <string>
#include <vector>
#include "glm/vec2.hpp"

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
		std::vector<glm::vec2> m_blockPattern;
		std::vector<glm::vec2> m_rotationPoints;

	public:
		// Width of the defining pattern of the Tetromino
		static const int GetPatternWidth()
		{
			return 3;
		}
		// Height of the defining pattern of the Tetromino
		static const int GetPatternHeight()
		{
			return 3;
		}
	public:
		static const std::string GetBlockModelPath()
		{
			return "./data/block/grey.obj";
		}

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
		Tetromino(const tetrominoType_t& tetrominoType) : m_tetrominoType(tetrominoType)
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
				if (registry.has<Components::Obstructable>(GetBlock(i)))
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
				if (registry.has<Components::Obstructable>(GetBlock(i)))
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
				if (registry.has<Components::Obstructable>(GetBlock(i)))
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
				if (registry.has<Components::Obstructable>(GetBlock(i)))
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
				if (registry.has<Components::Moveable>(GetBlock(i)))
				{
					auto& moveable = registry.get<Components::Moveable>(GetBlock(i));
					moveable.SetMovementState(movementState);
				}
			}
		}

		bool GetAreAllBlocksLocked(entt::registry& registry) const
		{
			for (int i = 0; i < 4; i++)
			{
				if (registry.has<Components::Moveable>(GetBlock(i)))
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

		glm::vec2 GetBlockOffsetCoordinates(int blockIndex, int rotationIndex = 0)
		{
			return m_blockPattern[blockIndex] - m_rotationPoints[rotationIndex];
		}

		glm::vec2 GetRotationPoints(int blockIndex)
		{
			return m_rotationPoints[blockIndex];
		}

		glm::vec2 GetBlockPattern(int blockIndex)
		{
			return m_blockPattern[blockIndex];
		}

		const moveDirection_t& GetNewOrientation(const rotationDirection_t& rotationDirection, const moveDirection_t& currentOrientation) const
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