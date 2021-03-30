#pragma once

#include "Globals.h"
#include "Components/Includes.h"
#include <entt/entity/registry.hpp>
#include <string>

enum class moveDirection_t;

namespace Components
{
	class Tetromino : public Component
	{
	protected:
		moveDirection_t m_orientation{ moveDirection_t::NORTH };
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

		const moveDirection_t& GetOrientation() const
		{
			return m_orientation;
		}

		const tetrominoType_t& GetType() const
		{
			return m_tetrominoType;
		}

		bool GetIsFallingObstructed() const
		{
			// What we actually want to do here is query all the Blocks that are part of this Tetromino.
			// TODO FIXME
			return false;
		}

		void SetIsFallingObstructed(bool isFallingObstructed)
		{
			// What we actually want to do here is signal all the Blocks that are part of this Tetromino.
			// TODO FIXME
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
	};
}