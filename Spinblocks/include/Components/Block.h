#pragma once

#include "Components/Component.h"
#include "Components/ReferenceEntity.h"
#include "Components/Coordinate.h"
#include <entt/entity/registry.hpp>

namespace Components
{
	class Block : public ReferenceEntity
	{
	public:
		Block (entt::entity entity) : ReferenceEntity(entity)
		{
		}
	};
}

class BlockLockData
{
protected:
	Components::Coordinate m_coordinates;
public:
	BlockLockData(Components::Coordinate coordinates) : m_coordinates(coordinates)
	{

	}
public:
	void GetCoordinates(const Components::Coordinate& coordinates)
	{
		m_coordinates = coordinates;
	}

	const Components::Coordinate& GetCoordinates() const
	{
		return m_coordinates;
	}
};