#include "Input/InputHandler.h"
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <iostream>

InputHandler::InputHandler()
{
	for(auto& key : keys)
	{
		UnBind(key.second);
	}

	// Setup the default controls.
	BindDefault(GLFW_KEY_ESCAPE, KeyInput::usercmdButton_t::UB_FORCE_QUIT);
#ifdef _DEBUG
	BindDefault(GLFW_KEY_1, KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_1);
	BindDefault(GLFW_KEY_2, KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_2);
	BindDefault(GLFW_KEY_3, KeyInput::usercmdButton_t::UB_DEBUG_SPAWN_3);
	BindDefault(GLFW_KEY_HOME, KeyInput::usercmdButton_t::UB_DEBUG_MOVE_UP);
	BindDefault(GLFW_KEY_DELETE, KeyInput::usercmdButton_t::UB_DEBUG_ROTATE_PLAY_AREA_COUNTERCLOCKWISE);
	BindDefault(GLFW_KEY_PAGE_DOWN, KeyInput::usercmdButton_t::UB_DEBUG_ROTATE_PLAY_AREA_CLOCKWISE);
	BindDefault(GLFW_KEY_Q, KeyInput::usercmdButton_t::UB_DEBUG_PROJECT_DOWN);
#endif
	BindDefault(GLFW_KEY_LEFT, KeyInput::usercmdButton_t::UB_MOVE_LEFT);
	BindDefault(GLFW_KEY_RIGHT, KeyInput::usercmdButton_t::UB_MOVE_RIGHT);
	BindDefault(GLFW_KEY_DOWN, KeyInput::usercmdButton_t::UB_SOFT_DROP);
	BindDefault(GLFW_KEY_UP, KeyInput::usercmdButton_t::UB_HARD_DROP);
	BindDefault(GLFW_KEY_Z, KeyInput::usercmdButton_t::UB_ROTATE_COUNTERCLOCKWISE);
	BindDefault(GLFW_KEY_X, KeyInput::usercmdButton_t::UB_ROTATE_CLOCKWISE);
	BindDefault(GLFW_KEY_PAUSE, KeyInput::usercmdButton_t::UB_PAUSE);
#ifdef _DEBUG
	BindDefault(GLFW_KEY_SPACE, KeyInput::usercmdButton_t::UB_HOLD);
#endif

	ResetBindings(); // Set the default controls.

}

void InputHandler::Bind(const int& key, const KeyInput::usercmdButton_t& button, const std::string& context)
{
	keys[key].keyDown = false;
	keys[key].prevKeyDown = false;
	keys[key].currentKeyDownBeginTime = 0.0;
	keys[key].currentKeyUpBeginTime = 0.0;
	keys[key].lastKeyDownRepeatTime = 0.0;
	keys[key].lastKeyUpRepeatTime = 0.0;
	keys[key].controls.SetControl(button, context);
}

void InputHandler::BindDefault(const int& key, const KeyInput::usercmdButton_t& button, const std::string& context)
{
	if (keys.find(key) == keys.end())
	{
		keyState ks;
		ks.defaultControls.SetControl(button, context);
		keys.insert({ key, ks });
	}
	else
	{
		keys[key].defaultControls.SetControl(button, context);
	}
}

void InputHandler::ResetBinding(const int& key)
{
	UnBind(key);

	ContextControl control = keys[key].defaultControls;
	if (control.IsSet())
	{
		std::vector<std::string> contexts;
		control.GetContexts(contexts);
		for(auto& context : contexts)
		{
			Bind(key, control.GetControl(context), context);
		}
	}
}

void InputHandler::ResetBindings()
{
	for(auto& key : keys)
	{
		ResetBinding(key.first);
	}
}

const ContextControl& InputHandler::GetControl(const int& key)
{
	return this->keys[key].controls;
}

/*const keyState& InputHandler::GetKeyState(const int& scankey, const string& context) const
{
	return keys[MapKey(scankey)];
}*/

void InputHandler::UnBind(const int& key, const std::string& context)
{
	UnBind(keys[key], context);
}

void InputHandler::UnBind(keyState& key, const std::string& context)
{
	key.keyDown = false;
	key.prevKeyDown = false;
	key.currentKeyDownBeginTime = 0.0;
	key.currentKeyUpBeginTime = 0.0;
	key.lastKeyDownRepeatTime = 0.0;
	key.lastKeyUpRepeatTime = 0.0;
	key.controls.UnSetAllControls();
}

void InputHandler::UnBindAll()
{
	for(auto& key : keys)
	{
		UnBind(key.second);
	}
}

std::map<int, keyState>& InputHandler::GetAllKeyStates()
{
	return keys;
}