#pragma once

#include <string>
#include <vector>
#include <stdexcept>

namespace KeyInput
{
	enum class usercmdButton_t
	{
		UB_NONE,

		UB_FORCE_QUIT,
		UB_DEBUG_SPAWN_1,
		UB_DEBUG_SPAWN_2,
		UB_DEBUG_MOVE_UP,
		UB_MOVE_LEFT,
		UB_MOVE_RIGHT,
		UB_SOFT_DROP,
		UB_HARD_DROP,
		UB_ROTATE_COUNTERCLOCKWISE,
		UB_ROTATE_CLOCKWISE,
		UB_HOLD,
		UB_PAUSE,
		UB_MAX_BUTTONS
	};

	struct userCmdString_t
	{
		const char* string;
		usercmdButton_t	button;
	};

	struct keyname_t {
		const char* name;
		const char* printable;
		int	keynum;
	};
}

namespace KeyInput
{
	const int GetKeyNumForName(const std::string& name);
	const usercmdButton_t& GetButtonForName(const std::string& name);
	const std::string GetKeyNameForNum(const int& keynum);
	const std::string GetNameForButton(const usercmdButton_t& button);
}