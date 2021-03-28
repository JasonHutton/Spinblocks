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
			m_blockPattern.push_back(glm::vec2(0, 2));
			m_blockPattern.push_back(glm::vec2(1, 2));
			m_blockPattern.push_back(glm::vec2(2, 2));
			m_blockPattern.push_back(glm::vec2(3, 2));
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