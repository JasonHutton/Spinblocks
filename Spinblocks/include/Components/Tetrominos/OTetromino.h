#pragma once

#include "Components/Tetrominos/Tetromino.h"

namespace Components
{
	class OTetromino : public Tetromino
	{
	public:
		OTetromino() :Tetromino(tetrominoType_t::O)
		{
		}
	};
}