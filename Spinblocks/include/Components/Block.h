#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Block : public ReferenceEntity
	{
	protected:
		bool m_isFallingObstructed = false;

	public:
		Block (entt::entity entity) : ReferenceEntity(entity)
		{
		}

		bool GetIsFallingObstructed() const
		{
			return m_isFallingObstructed;
		}

		void SetIsFallingObstructed(bool isFallingObstructed)
		{
			m_isFallingObstructed = isFallingObstructed;
		}
	};
}