#include "CachedTagLookup.h"

entt::entity CachedTagLookup::Get(entt::registry& registry, const std::string& tag)
{
	entt::entity ent = entt::null;

	try
	{
		ent = m_lookupTable.at(tag);
	}
	catch (std::out_of_range ex)
	{
		auto tagView = registry.view<Components::Tag>();
		for (auto entity : tagView)
		{
			auto& tagRef = tagView.get<Components::Tag>(entity);
			if (tagRef.Get() == tag)
			{
				ent = entity;
				m_lookupTable.emplace(tag, ent);

				break;
			}
		}
	}

	return ent;
}

CachedTagLookup cachedTagLookup;