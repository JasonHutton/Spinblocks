#include "AudioManager.h"

void AudioManager::Init()
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_system);      // Create the main system object.
	if (result != FMOD_OK)
	{
		//std::cout << "FMOD error! (" << static_cast<int>(result) << ") " << FMOD_ErrorString(result) << std::endl;
		throw std::runtime_error("FMOD error! System object not created!");
	}

	result = m_system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	if (result != FMOD_OK)
	{
		//std::cout << "FMOD error! (" << static_cast<int>(result) << ") " << FMOD_ErrorString(result) << std::endl;
		throw std::runtime_error("FMOD error! FMOD not initialized!");
	}

	m_system->getMasterChannelGroup(&m_masterChannelGroup);
	m_system->createChannelGroup(GetNameOfAudioChannel(audioChannel_t::SOUND).c_str(), &m_soundChannelGroup);
	m_system->createChannelGroup(GetNameOfAudioChannel(audioChannel_t::MUSIC).c_str(), &m_musicChannelGroup);
}

void AudioManager::AddPath(const audioAsset_t& key, const std::string& path)
{
	m_soundPaths.emplace(key, path);
}

bool AudioManager::AreAllAssetsLoaded() const
{
	for(auto asset : m_soundPaths)
	{ 
		if (asset.second.empty())
			continue; // Ignore if it's not been setup.

		if (!IsSoundLoaded(asset.first))
			return false; // No. Abort immediately.
	}

	return true;
}

const std::string& AudioManager::GetSoundPath(const audioAsset_t& key) const
{
	return m_soundPaths.at(key);
}

audioData_t AudioManager::GetSound(const audioAsset_t& asset, const audioChannel_t& audioChannel, const bool& looping, const bool& nonblocking)
{
	audioData_t soundData;

	try
	{
		soundData = m_lookupTable.at(asset);
	}
	catch (std::out_of_range ex)
	{
		soundData.asset = asset;
		soundData.path = GetSoundPath(asset);
		soundData.audioChannel = audioChannel;
		soundData.looping = looping;

		FMOD_MODE mode = FMOD_DEFAULT;
		mode |= FMOD_2D;
		mode |= soundData.looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		mode |= FMOD_CREATECOMPRESSEDSAMPLE;
		if (nonblocking)
		{
			mode |= FMOD_NONBLOCKING;
		}

		FMOD_RESULT result;
		result = m_system->createSound(soundData.path.c_str(), mode, nullptr, &soundData.sound);
		if (result != FMOD_OK)
		{
			throw std::runtime_error("FMOD error! Unable to create sound!");
		}
		m_lookupTable.emplace(asset, soundData);
	}

	return soundData;
}

void AudioManager::PlaySound(const audioData_t& audioData) const
{
	if (audioData.path.empty())
		return;

	FMOD_RESULT result;
	FMOD::ChannelGroup* channel = NULL;
	switch (audioData.audioChannel)
	{
	case audioChannel_t::SOUND:
		channel = m_soundChannelGroup;
		break;
	case audioChannel_t::MUSIC:
		channel = m_musicChannelGroup;
		break;
	default:
		channel = m_masterChannelGroup;
		break;
	}

	result = m_system->playSound(audioData.sound, channel, false, nullptr);
	if (result != FMOD_OK)
	{
		throw std::runtime_error("FMOD error! Unable to play sound!");
	}
}

void AudioManager::SetChannelVolume(const audioChannel_t& audioChannel, const float& volume)
{
	FMOD::ChannelGroup* channel = NULL;
	switch (audioChannel)
	{
	case audioChannel_t::SOUND:
		channel = m_soundChannelGroup;
		break;
	case audioChannel_t::MUSIC:
		channel = m_musicChannelGroup;
		break;
	default:
		channel = m_masterChannelGroup;
		break;
	}

	channel->setVolume(volume);
}

float AudioManager::GetChannelVolume(const audioChannel_t& audioChannel) const
{
	FMOD::ChannelGroup* channel = NULL;
	switch (audioChannel)
	{
	case audioChannel_t::SOUND:
		channel = m_soundChannelGroup;
		break;
	case audioChannel_t::MUSIC:
		channel = m_musicChannelGroup;
		break;
	default:
		channel = m_masterChannelGroup;
		break;
	}

	float volume;
	channel->getVolume(&volume);

	return volume;
}

bool AudioManager::IsSoundLoaded(const audioAsset_t& asset) const
{
	audioData_t soundData;

	try
	{
		soundData = m_lookupTable.at(asset);
	}
	catch (std::out_of_range ex)
	{
		return false;
	}

	FMOD_RESULT result;
	FMOD_OPENSTATE openState;
	unsigned int percentBuffered;
	bool starving;
	bool diskbusy;
	
	result = soundData.sound->getOpenState(&openState, &percentBuffered, &starving, &diskbusy);

	if (result == FMOD_OK && (openState == FMOD_OPENSTATE_READY || openState == FMOD_OPENSTATE_PLAYING))
		return true;

	return false;
}

AudioManager audioManager;
