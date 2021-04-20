#pragma once

//namespace Systems
//{
	const int PlayAreaWidth = 10; // This shouldn't be done this way, but for now this is okay. FIXME TODO // Width of the play area when north facing
	const int PlayAreaHeight = 20; // This shouldn't be done this way, but for now this is okay. FIXME TODO // Height of the play area when north facing
	const int BufferAreaDepth = 5; // This shouldn't be done this way, but for now this is okay. FIXME TODO // Depth of the buffer area around the play area, on all sides.
	const double KeyRepeatDelay = 0.3; // Delay before starting to repeat.
	const double KeyRepeatRate = 0.5 / PlayAreaWidth; // Delay between repeats. // This does not change when changing orientation.
	const double FallSpeed = 1.0; // Base fall speed, time it takes to move 1 line.
	inline double lastFallUpdate = 0.0;
	inline double lastLockdownTime = 0.0;
	const double generationTimeDelay = 0.2; // Delay after last lockdown before a new generation occurs. (And a Tetromino is spawned into the play area matrix.)
	inline double lastBoardRotationTime = 0.0;
	const double lockdownDelay = 0.5;
	const unsigned int cellWidth = 25;
	const unsigned int cellHeight = 25;
//}