#pragma once

#include "Components/Component.h"

#include <deque>
#include <random>

namespace Components
{
	class Bag : public Component
	{
	protected:
		std::deque<tetrominoType_t> m_tetrominos;
		std::mt19937 m_pseudorandom;

	public:
		Bag() : m_pseudorandom((std::random_device())())
		{
		}

		tetrominoType_t PopTetromino()
		{
			if (m_tetrominos.empty())
				FillBag();

			tetrominoType_t poppedTet = m_tetrominos.front();
			m_tetrominos.pop_front();
			return poppedTet;
		}

	protected:
		void FillBag()
		{
			if (m_tetrominos.empty())
			{
				// One of each Tetromino type.
				m_tetrominos.push_back(tetrominoType_t::I);
				m_tetrominos.push_back(tetrominoType_t::J);
				m_tetrominos.push_back(tetrominoType_t::L);
				m_tetrominos.push_back(tetrominoType_t::O);
				m_tetrominos.push_back(tetrominoType_t::S);
				m_tetrominos.push_back(tetrominoType_t::T);
				m_tetrominos.push_back(tetrominoType_t::Z);

				std::shuffle(m_tetrominos.begin(), m_tetrominos.end(), m_pseudorandom);
			}
		}
	};
}