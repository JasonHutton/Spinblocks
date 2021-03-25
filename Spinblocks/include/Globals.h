#pragma once

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

enum class moveDirection_t
{
	NORTH,
	SOUTH,
	EAST,
	WEST
};