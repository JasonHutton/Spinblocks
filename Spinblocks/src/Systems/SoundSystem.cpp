#include "Systems/SoundSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include "AudioManager.h"



namespace Systems
{
	void SoundSystem(entt::registry& registry, const bool& aPieceMoved, const statesChanged_t& statesChanged, const int& linesMatched)
	{
		if (false)//aPieceMoved)
		{
			audioData_t audioDataPieceMove = audioManager.GetSound("./data/audio/sounds/Puntuation Sound 1.mp3", audioChannel_t::SOUND, false, true);
			audioManager.PlaySound(audioDataPieceMove);
		}

		
		if(false)//statesChanged.pieceLocked)
		{
			audioData_t audioDataPieceMove = audioManager.GetSound("./data/audio/sounds/Puntuation Sound 1.mp3", audioChannel_t::SOUND, false, true);
			audioManager.PlaySound(audioDataPieceMove);
		}

		if (linesMatched > 0)
		{
			audioData_t audioDataLineClear = audioManager.GetSound("./data/audio/sounds/Fading Block 2.mp3", audioChannel_t::SOUND, false, true);
			audioManager.PlaySound(audioDataLineClear);
		}
	}
}