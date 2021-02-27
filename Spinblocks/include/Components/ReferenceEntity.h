#pragma once

#include "Component.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class ReferenceEntity : public Component
	{
	protected:
		entt::entity m_entity{ entt::null };

	public:
		ReferenceEntity(entt::entity entity) : m_entity(entity)
		{
		}

		const entt::entity& Get() const
		{
			return m_entity;
		}

		void Set(const entt::entity entity)
		{
			m_entity = entity;
		}
	};
}