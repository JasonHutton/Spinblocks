#include "Systems/CompletionSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	void CompletionSystem(entt::registry& registry, double currentFrameTime, int linesCleared)
	{
		if (linesCleared == 0)
			return;

		LinesClearedTotal += linesCleared;

		// (0.8 - ((level - 1) * 0.007))^(level-1)
		while (linesCleared > 0)
		{
			switch (linesCleared)
			{
			case 1:
				GameScore += 100 * GameLevel;
				linesCleared -= 1;
				break;
			case 2:
				GameScore += 300 * GameLevel;
				linesCleared -= 2;
				break;
			case 3:
				GameScore += 500 * GameLevel;
				linesCleared -= 3;
				break;
			case 4:
				GameScore += 800 * GameLevel;
				linesCleared -= 4;
				break;
			default:
				// This needs better looking at. Just getting the next in the fibonacci sequence here
				GameScore += 1300 * GameLevel;
				linesCleared -= linesCleared;
				break;
			}
		}

		if (LinesClearedTotal > LevelGoal)
		{
			GameLevel += 1;
			LevelGoal += (GameLevel * LevelGoalIncrement);
		}

		FallSpeed = pow((0.8 - ((static_cast<double>(GameLevel) - 1) * 0.007)), static_cast<double>(GameLevel) - 1);

		/*
		cout << "Score: " << GameScore << endl;
		cout << "Game Level: " << GameLevel << endl;
		cout << "Total Lines Cleared: " << LinesClearedTotal << " Level Goal: " << LevelGoal << endl;
		cout << "Fall Speed: " << FallSpeed << endl;
		*/
	}
}