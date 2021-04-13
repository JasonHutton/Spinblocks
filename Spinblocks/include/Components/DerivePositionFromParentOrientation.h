#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"

namespace Components
{
	// This is just a bad name/concept in general.
	class DerivePositionFromParentOrientation : public ReferenceEntity
	{
	protected:

	public:
		DerivePositionFromParentOrientation(entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}