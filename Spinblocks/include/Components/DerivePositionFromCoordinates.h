#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class DerivePositionFromCoordinates : public ReferenceEntity
	{
	protected:
		glm::vec3 m_offset;

	public:
		DerivePositionFromCoordinates(entt::entity entity = entt::null, const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f)) : m_offset(offset), ReferenceEntity(entity)
		{
		}

		DerivePositionFromCoordinates(entt::entity entity, const glm::vec2& offset) : ReferenceEntity(entity)
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