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