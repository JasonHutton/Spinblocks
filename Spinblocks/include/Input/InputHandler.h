#pragma once

#include "KeyInput.h"
#include "ContextControl.h"

struct keyState
{
	bool	keyDown;
	bool	prevKeyDown;
	int		repeats;
	ContextControl controls;
	ContextControl defaultControls;
};

class InputHandler
{
public:
	InputHandler();

	void Bind(const int& key, const KeyInput::usercmdButton_t& button, const std::string& context = "");
	void BindDefault(const int& key, const KeyInput::usercmdButton_t& button, const std::string& context = "");
	void ResetBinding(const int& key);
	void ResetBindings();
	const ContextControl& GetControl(const int& key);
	//const keyState& GetKeyState(const int& scankey, const string& context = "") const;
	void UnBind(const int& key, const std::string& context = ""); // Maybe want to force wildcards for contextual UNbinding?
	void UnBind(keyState& key, const std::string& context = ""); // Maybe want to force wildcards for contextual UNbinding?
	void UnBindAll();

	std::map<int, keyState>& GetAllKeyStates();

private:
	std::map<int, keyState> keys; // The key value NEEDS to contain all possible GLFW_KEY_* macros up to GLFW_KEY_LAST bearing in mind GLFW_KEY_UNKNOWN is -1...
};
