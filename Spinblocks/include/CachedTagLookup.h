#pragma once

#include <entt/entity/registry.hpp>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "Components/Includes.h"

class CachedTagLookup
{
protected:
	std::unordered_map<std::string, entt::entity> m_lookupTable;

public:
	CachedTagLookup()
	{
	}

	entt::entity Get(entt::registry& registry, const std::string& tag);
	
	void Clear()
	{
		m_lookupTable.clear();
	}
};

extern CachedTagLookup cachedTagLookup;