#pragma once

#include <string>

inline struct displayData_t
{
	int x{ 800 };
	int y{ 600 };
	std::string title{ "Spinblocks" };
} displayData;

// Ensure GetTagFromContainerType() and this enum are in sync
enum class containerType_t
{
	PLAY_AREA = 0,
	MATRIX,
	BUFFER,
	BAG_AREA
};

enum class movePiece_t
{
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	SOFT_DROP,
	HARD_DROP
};

enum class rotatePiece_t
{
	ROTATE_CLOCKWISE,
	ROTATE_COUNTERCLOCKWISE
};

enum class rotationDirection_t
{
	CLOCKWISE,
	COUNTERCLOCKWISE
};

enum class moveDirection_t
{
	NORTH,
	SOUTH,
	EAST,
	WEST
};

enum class tetrominoType_t
{
	T,
	J,
	Z,
	O,
	L,
	S,
	I
};