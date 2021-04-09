#pragma once

#include "Components/Tetrominos/Tetromino.h"

namespace Components
{
	class OTetromino : public Tetromino
	{
	public:
		// Width of the defining pattern of the Tetromino
		const int GetPatternWidth() override
		{
			return 3;
		}
		// Height of the defining pattern of the Tetromino
		const int GetPatternHeight() override
		{
			return 3;
		}
	public:
		const std::string GetBlockModelPath() override
		{
			return "./data/block/yellow.obj";
		}

	protected:
		void DefineBlockPattern() override
		{
			AddBlockPattern(0, moveDirection_t::NORTH, rotationDirection_t::NONE, { glm::vec2(1,1), glm::vec2(2,1), glm::vec2(1,2), glm::vec2(2,2) });
			AddBlockPattern(0, moveDirection_t::NORTH, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::NORTH, rotationDirection_t::COUNTERCLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));

			AddBlockPattern(0, moveDirection_t::EAST, rotationDirection_t::NONE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::EAST, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::EAST, rotationDirection_t::COUNTERCLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));

			AddBlockPattern(0, moveDirection_t::SOUTH, rotationDirection_t::NONE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::SOUTH, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::SOUTH, rotationDirection_t::COUNTERCLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));

			AddBlockPattern(0, moveDirection_t::WEST, rotationDirection_t::NONE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::WEST, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::WEST, rotationDirection_t::COUNTERCLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
		}

		void DefineRotationPoints() override
		{
			m_rotationPoints.push_back(glm::vec2(1, 1));
			m_rotationPoints.push_back(glm::vec2(1, 1));
			m_rotationPoints.push_back(glm::vec2(1, 1));
			m_rotationPoints.push_back(glm::vec2(1, 1));
			m_rotationPoints.push_back(glm::vec2(1, 1));
		}

	public:
		OTetromino() :Tetromino(tetrominoType_t::O)
		{
			DefineBlockPattern();
			DefineRotationPoints();
		}
	};
}