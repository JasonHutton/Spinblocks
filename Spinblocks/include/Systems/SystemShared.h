#pragma once

//namespace Systems
//{
	const int PlayAreaWidth = 10;
	const double KeyRepeatDelay = 0.3; // Delay before starting to repeat.
	const double KeyRepeatRate = 0.5 / PlayAreaWidth; // Delay between repeats.
	const double FallSpeed = 1.0; // Base fall speed, time it takes to move 1 line.
	inline double lastFallUpdate = 0.0;
	const double lockdownDelay = 0.5;
//}