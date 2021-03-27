#pragma once

#include "Components/Tetrominos/Tetromino.h"

namespace Components
{
	class ITetromino : public Tetromino
	{
	public:
		// Width of the defining pattern of the Tetromino
		static const int PatternWidth = 4;
		// Height of the defining pattern of the Tetromino
		static const int PatternHeight = 4;

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