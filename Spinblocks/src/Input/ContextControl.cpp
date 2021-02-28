#include "Input/ContextControl.h"
#include <algorithm>

void ContextControl::SetControl(const KeyInput::usercmdButton_t& control, const std::string& context)
{
	this->m_controlContexts[context] = control;
}

const KeyInput::usercmdButton_t ContextControl::GetControl(const std::string& context) const
{
	std::map<std::string, KeyInput::usercmdButton_t>::const_iterator it = this->m_controlContexts.find(context);
	if (it != this->m_controlContexts.end())
		return it->second;
	else
		return KeyInput::usercmdButton_t::UB_NONE;
}

std::vector<std::string>& ContextControl::GetContexts(std::vector<std::string>& contexts) const
{
	for (std::map<std::string, KeyInput::usercmdButton_t>::const_iterator it = this->m_controlContexts.begin(); it != this->m_controlContexts.end(); it++)
	{
		if (find(contexts.begin(), contexts.end(), it->first) == contexts.end())
		{
			contexts.push_back(it->first);
		}
	}
	return contexts;
}

const bool ContextControl::UnSetControl(const std::string& context)
{
	std::map<std::string, KeyInput::usercmdButton_t>::const_iterator it = this->m_controlContexts.find(context);
	if (it != this->m_controlContexts.end())
	{
		this->m_controlContexts.erase(it);
		return true;
	}

	return false;
}

void ContextControl::UnSetAllControls()
{
	this->m_controlContexts.clear();
}

const bool ContextControl::IsSet() const
{
	return !this->m_controlContexts.empty();
}