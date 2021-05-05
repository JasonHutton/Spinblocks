#pragma once

#include <string>

#include <fmod/fmod.hpp>

inline struct displayData_t
{
	int x{ 800 };
	int y{ 600 };
	std::string title{ "Spinblocks" };
} displayData;

enum class gameState_t
{
	INIT,
	MENU,
	PLAY,
	GAME_OVER
};

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
	ROTATE_COUNTERCLOCKWISE,
	ROTATE_CLOCKWISE
};

enum class rotationDirection_t
{
	COUNTERCLOCKWISE,
	NONE,
	CLOCKWISE
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

enum class spawnType_t
{
	WIDTH3, // T, L, J, S, Z
	ITETROMINO, // I
	OTETROMINO // O
};

// Ensure GetNameOfAudioChannel() and this enum are in sync
enum class audioChannel_t
{
	SOUND,
	MUSIC,
	MASTER
};

// Ensure GetNameOfAudioAsset() and this enum are in sync
enum class audioAsset_t
{
	SOUND_MOVE,
	SOUND_ROTATE,
	SOUND_LOCK,
	SOUND_HARD_DROP,
	SOUND_LINE_CLEAR,
	MUSIC_MENU,
	MUSIC_GAMEPLAY1,
	MUSIC_GAMEPLAY2
};

struct audioData_t
{
	audioAsset_t asset;
	std::string path{ "" };
	FMOD::Sound* sound = NULL;
	audioChannel_t audioChannel = audioChannel_t::SOUND;
	bool looping = false;
};

struct statesChanged_t
{
	bool pieceMoved = false;
	bool pieceLocked = false;
	bool peiceHardDropped = false;
};