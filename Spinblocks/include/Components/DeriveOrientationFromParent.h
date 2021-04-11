#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include "glm/vec3.hpp"

namespace Components
{
	class DeriveOrientationFromParent : public ReferenceEntity
	{
	protected:
		float m_orientationOffset;
		glm::vec3 m_axisOffset;

	public:
		DeriveOrientationFromParent(entt::entity entity, const float& orientationOffset = 0.0f, const glm::vec3& axisOffset = glm::vec3(0.0f, 0.0f, 0.0f)) : 
			m_orientationOffset(orientationOffset), m_axisOffset(axisOffset), ReferenceEntity(entity)
		{
		}

	public:
		const float& GetOffset() const
		{
			return m_orientationOffset;
		}

		void SetOffset(const float& orientationOffset)
		{
			m_orientationOffset = orientationOffset;
		}

		const glm::vec3& GetAxisOffset() const
		{
			return m_axisOffset;
		}

		void SetAxisOffset(const glm::vec3& axisOffset)
		{
			m_axisOffset = axisOffset;
		}
	};
}