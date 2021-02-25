#include "Components/Coordinate.h"

std::string to_string(Components::Coordinate coord)
{
	std::ostringstream ss;
	ss << coord;
	return ss.str();
}