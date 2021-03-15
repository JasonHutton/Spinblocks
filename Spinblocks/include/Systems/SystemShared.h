#pragma once

//namespace Systems
//{
	const int PlayAreaWidth = 10; // This shouldn't be done this way, but for now this is okay. FIXME TODO
	const int PlayAreaHeight = 20; // This shouldn't be done this way, but for now this is okay. FIXME TODO
	const double KeyRepeatDelay = 0.3; // Delay before starting to repeat.
	const double KeyRepeatRate = 0.5 / PlayAreaWidth; // Delay between repeats.
	const double FallSpeed = 1.0; // Base fall speed, time it takes to move 1 line.
	inline double lastFallUpdate = 0.0;
	const double lockdownDelay = 0.5;
//}