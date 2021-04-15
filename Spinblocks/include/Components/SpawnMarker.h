#pragma once

#include "Components/Component.h"
#include "Components/Marker.h"
#include <entt/entity/registry.hpp>

enum class spawnType_t;

namespace Components
{
	class SpawnMarker : public Marker
	{
	protected:
		spawnType_t m_spawnType;

	public:
		SpawnMarker (entt::entity entity, spawnType_t spawnType) : m_spawnType(spawnType), Marker(entity)
		{
		}

		const spawnType_t& GetSpawnType() const
		{
			return m_spawnType;
		}
	};
}