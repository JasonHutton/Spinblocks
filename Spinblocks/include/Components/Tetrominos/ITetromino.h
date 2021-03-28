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

		}

		void DefineRotationPoints() override
		{

		}
	public:
		ITetromino() :Tetromino(tetrominoType_t::I)
		{
			DefineBlockPattern();
			DefineRotationPoints();
		}
	};
}