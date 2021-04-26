#pragma once

//namespace Systems
//{
	const int PlayAreaWidth = 10; // This shouldn't be done this way, but for now this is okay. FIXME TODO // Width of the play area when north facing
	const int PlayAreaHeight = 20; // This shouldn't be done this way, but for now this is okay. FIXME TODO // Height of the play area when north facing
	const int BufferAreaDepth = 5; // This shouldn't be done this way, but for now this is okay. FIXME TODO // Depth of the buffer area around the play area, on all sides.
	const double KeyRepeatDelay = 0.3; // Delay before starting to repeat.
	const double KeyRepeatRate = 0.5 / PlayAreaWidth; // Delay between repeats. // This does not change when changing orientation.
	inline int GameScore = 0;
	const int StartGameLevel = 1;
	inline int GameLevel = 1;
	const int LevelGoalIncrement = 5;
	const int StartLevelGoal = 5;
	inline int LevelGoal = 5;
	inline int LinesClearedTotal = 0;
	inline double FallSpeed = 1.0; // // Base fall speed, time it takes to move 1 line.... (0.8 - ((level - 1) * 0.007))^(level-1)
	inline double lastFallUpdate = 0.0;
	inline double lastLockdownTime = 0.0;
	const double generationTimeDelay = 0.2; // Delay after last lockdown before a new generation occurs. (And a Tetromino is spawned into the play area matrix.)
	inline double lastBoardRotationTime = 0.0;
	const double lockdownDelay = 0.5;
	const unsigned int cellWidth = 25;
	const unsigned int cellHeight = 25;
	const unsigned int minimumLinesMatchedToTriggerBoardRotation = 2;
//}