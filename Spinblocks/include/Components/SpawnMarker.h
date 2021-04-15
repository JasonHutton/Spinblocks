#pragma once

#include "Components/Component.h"
#include "Components/Marker.h"
#include <entt/entity/registry.hpp>

enum class spawnType_t;
enum class tetrominoType_t;

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

		const bool ValidForTetrominoType(const tetrominoType_t& tetrominoType) const
		{
			switch (GetSpawnType())
			{
			case spawnType_t::ITETROMINO:
				switch (tetrominoType)
				{
				case tetrominoType_t::I:
					return true;
				default:
					return false;
				}
				break;
			case spawnType_t::OTETROMINO:
				switch (tetrominoType)
				{
				case tetrominoType_t::O:
					return true;
				default:
					return false;
				}
				break;
			default: // spawnType_t::WIDTH3
				switch (tetrominoType)
				{
				case tetrominoType_t::J:
				case tetrominoType_t::L:
				case tetrominoType_t::S:
				case tetrominoType_t::T:
				case tetrominoType_t::Z:
					return true;
				default: // I, O
					return false;
				}
				break;
			}
		}
	};
}