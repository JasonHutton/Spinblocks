#include "Components/Coordinate.h"

namespace Components
{
	std::ostream& operator<<(std::ostream& out, const Coordinate& coord)
	{
		out << coord.m_coordinate.x << "," << coord.m_coordinate.y;
		return out;
	}
}