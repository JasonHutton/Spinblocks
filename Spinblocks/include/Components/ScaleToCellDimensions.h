#pragma once

#include "Component.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class ScaleToCellDimensions : public Component
	{
	protected:
		entt::entity m_parent{ entt::null };

	public:
		ScaleToCellDimensions(entt::entity parent) : m_parent(parent)
		{
		}

		const entt::entity& Get() const
		{
			return m_parent;
		}

		void Set(const entt::entity parent)
		{
			m_parent = parent;
		}
	};
}