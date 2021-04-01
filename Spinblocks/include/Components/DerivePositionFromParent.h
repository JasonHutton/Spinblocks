#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class DerivePositionFromParent : public ReferenceEntity
	{
	protected:
		glm::vec3 m_offset;

	public:
		DerivePositionFromParent(entt::entity entity, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f)) : m_offset(offset), ReferenceEntity(entity)
		{
		}

	public:
		DerivePositionFromParent(entt::entity entity, const glm::vec2& offset) : ReferenceEntity(entity)
		{
			SetOffset(offset);
		}

		const glm::vec3& GetOffset() const
		{
			return m_offset;
		}

		void SetOffset(const glm::vec3& offset)
		{
			m_offset = offset;
		}

		void SetOffset(const glm::vec2& offset)
		{
			m_offset = glm::vec3(offset.x, offset.y, 0.0f);
		}
	};
}