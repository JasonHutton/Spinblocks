#pragma once

#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <unordered_map>
#include <iostream>

#include "Globals.h"
#include "Utility.h"

enum class audioChannel_t;;

class AudioManager
{
protected:
	//std::unordered_map<std::string, FMOD::Sound*> m_lookupTable;
	std::unordered_map<std::string, audioData_t> m_lookupTable;
	FMOD::System* m_system = NULL;

	FMOD::ChannelGroup* m_masterChannelGroup = NULL;
	FMOD::ChannelGroup* m_soundChannelGroup = NULL;
	FMOD::ChannelGroup* m_musicChannelGroup = NULL;

public:
	AudioManager()
	{
		Init();
	}

	~AudioManager()
	{
		m_system->release();
	}

	void Init();


	// This is simplified. We probably want to be looking up based on various flags, not just the path. Might need digging into the FMOD documentation to determine how best to do it though. FIXME TODO
	audioData_t GetSound(const std::string& path, const audioChannel_t& audioChannel, const bool& looping, const bool& nonblocking);
	void PlaySound(const audioData_t& audioData) const;

	void Clear()
	{
		m_lookupTable.clear();
	}

	void Update()
	{
		m_system->update();
	}

	void SetChannelVolume(const audioChannel_t& audioChannel, const float& volume);
	float GetChannelVolume(const audioChannel_t& audioChannel) const;
	bool IsSoundLoaded(const std::string& path) const;
};

extern AudioManager audioManager;