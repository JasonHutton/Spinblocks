#pragma once

#include "Components/Tetrominos/Tetromino.h"

namespace Components
{
	class ITetromino : public Tetromino
	{
	public:
		// Width of the defining pattern of the Tetromino
		static const int GetPatternWidth()
		{
			return 4;
		}
		// Height of the defining pattern of the Tetromino
		static const int GetPatternHeight()
		{
			return 4;
		}
	public:
		static const std::string GetBlockModelPath()
		{
			return "./data/block/lightblue.obj";
		}

	protected:
		void DefineBlockPattern() override
		{
			AddBlockPattern(0, moveDirection_t::NORTH, rotationDirection_t::NONE, { glm::vec2(0,2), glm::vec2(1,2), glm::vec2(2,2), glm::vec2(3,2) });
			AddBlockPattern(0, moveDirection_t::NORTH, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::EAST, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::NORTH, rotationDirection_t::COUNTERCLOCKWISE, std::make_tuple(0, moveDirection_t::WEST, rotationDirection_t::NONE));

			AddBlockPattern(0, moveDirection_t::EAST, rotationDirection_t::NONE, { glm::vec2(2,0), glm::vec2(2,1), glm::vec2(2,2), glm::vec2(2,3) });
			AddBlockPattern(0, moveDirection_t::EAST, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::SOUTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::EAST, rotationDirection_t::COUNTERCLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));

			AddBlockPattern(0, moveDirection_t::SOUTH, rotationDirection_t::NONE, { glm::vec2(0,1), glm::vec2(1,1), glm::vec2(2,1), glm::vec2(3,1) });
			AddBlockPattern(0, moveDirection_t::SOUTH, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::WEST, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::SOUTH, rotationDirection_t::COUNTERCLOCKWISE, std::make_tuple(0, moveDirection_t::EAST, rotationDirection_t::NONE));

			AddBlockPattern(0, moveDirection_t::WEST, rotationDirection_t::NONE, { glm::vec2(1,0), glm::vec2(1,1), glm::vec2(1,2), glm::vec2(1,3) });
			AddBlockPattern(0, moveDirection_t::WEST, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::NORTH, rotationDirection_t::NONE));
			AddBlockPattern(0, moveDirection_t::WEST, rotationDirection_t::CLOCKWISE, std::make_tuple(0, moveDirection_t::SOUTH, rotationDirection_t::NONE));
		}

		void DefineRotationPoints() override
		{
			m_rotationPoints.push_back(glm::vec2(1, 2));
			m_rotationPoints.push_back(glm::vec2(0, 2));
			m_rotationPoints.push_back(glm::vec2(3, 2));
			m_rotationPoints.push_back(glm::vec2(0, 2));
			m_rotationPoints.push_back(glm::vec2(3, 2));
		}
	public:
		ITetromino() :Tetromino(tetrominoType_t::I)
		{
			DefineBlockPattern();
			DefineRotationPoints();
		}
	};
}