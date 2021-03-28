#pragma once

#include "Components/Tetrominos/Tetromino.h"

namespace Components
{
	class OTetromino : public Tetromino
	{
	protected:
		void DefineBlockPattern() override
		{
			m_blockPattern.push_back(glm::vec2(1, 1));
			m_blockPattern.push_back(glm::vec2(2, 1));
			m_blockPattern.push_back(glm::vec2(1, 2));
			m_blockPattern.push_back(glm::vec2(2, 2));
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
		static const std::string GetBlockModelPath()
		{
			return "./data/block/yellow.obj";
		}

	public:
		OTetromino() :Tetromino(tetrominoType_t::O)
		{
			DefineBlockPattern();
			DefineRotationPoints();
		}
	};
}