#include "Systems/SoundSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include "AudioManager.h"



namespace Systems
{
	void SoundSystem(entt::registry& registry, const bool& aPieceMoved, const statesChanged_t& statesChanged, const int& linesMatched)
	{
		if (false)// aPieceMoved)
		{
			audioData_t audioDataPieceMove = audioManager.GetSound(audioAsset_t::SOUND_MOVE, audioChannel_t::SOUND, false, true);
			audioManager.PlaySound(audioDataPieceMove);
		}

		if(statesChanged.pieceLocked || statesChanged.peiceHardDropped)
		{
			audioData_t audioDataPieceMove = audioManager.GetSound(audioAsset_t::SOUND_LOCK, audioChannel_t::SOUND, false, true);
			audioManager.PlaySound(audioDataPieceMove);
		}

		if (linesMatched > 0)
		{
			audioData_t audioDataLineClear = audioManager.GetSound(audioAsset_t::SOUND_LINE_CLEAR, audioChannel_t::SOUND, false, true);
			audioManager.PlaySound(audioDataLineClear);
		}
	}
}