#pragma once

#include "Globals.h"
#include "Components/Includes.h"
#include <entt/entity/registry.hpp>

enum class moveDirection_t;

namespace Components
{
	class Tetromino : public Component
	{
	protected:
		moveDirection_t m_orientation{ moveDirection_t::NORTH };
		tetrominoType_t m_tetrominoType;

		std::vector<entt::entity> m_blocks;
		std::vector<Components::Coordinate> m_rotationPoints;

		// virtual method to determine pattern based on table and rotation adn etc

	public:
		Tetromino(const tetrominoType_t& tetrominoType) : m_tetrominoType(tetrominoType)
		{
		}

		const moveDirection_t& GetOrientation() const
		{
			return m_orientation;
		}

		const tetrominoType_t& GetType() const
		{
			return m_tetrominoType;
		}

		bool GetIsFallingObstructed() const
		{
			// What we actually want to do here is query all the Blocks that are part of this Tetromino.
			// TODO FIXME
			return false;
		}

		void SetIsFallingObstructed(bool isFallingObstructed)
		{
			// What we actually want to do here is signal all the Blocks that are part of this Tetromino.
			// TODO FIXME
		}
	};
}