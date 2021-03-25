#pragma once

#include "Component.h"
#include "Components/ReferenceEntity.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Obstructable : public ReferenceEntity
	{
	protected:
		bool m_isObstructed = false;
		double m_lastObstructedTime = 0.0;

	public:
		Obstructable(entt::entity entity) : ReferenceEntity(entity)
		{
		}

		bool GetIsObstructed() const
		{
			return m_isObstructed;
		}

		void SetIsObstructed(bool isObstructed)
		{
			m_isObstructed = isObstructed;
		}

		double GetLastObstructedTime() const
		{
			return m_lastObstructedTime;
		}

		void SetLastObstructedTime(double lastObstructedTime)
		{
			m_lastObstructedTime = lastObstructedTime;
		}
	};
}