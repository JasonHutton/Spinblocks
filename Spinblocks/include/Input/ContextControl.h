#pragma once

#include <string>
#include <map>
#include <vector>
#include "KeyInput.h"

class ContextControl
{
public:
	void SetControl(const KeyInput::usercmdButton_t& control, const std::string& context = "");
	const KeyInput::usercmdButton_t GetControl(const std::string& context = "") const;
	std::vector<std::string>& GetContexts(std::vector<std::string>& contexts) const;
	const bool UnSetControl(const std::string& context = "");
	void UnSetAllControls();
	const bool IsSet() const;

private:
	std::map<std::string, KeyInput::usercmdButton_t> m_controlContexts;
};
