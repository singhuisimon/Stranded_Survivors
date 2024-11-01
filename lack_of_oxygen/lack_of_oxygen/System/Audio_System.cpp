/**
 * @file Audio_System.cpp
 * @brief Define of the Audio_System class for managing audio playback using FMOD.
 * @author Amanda Leow Boon Suan (100%)
 * @date September 27, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "Audio_System.h"

namespace lof {

	void Audio_System::initializegroups() {
		core_system->getMasterChannelGroup(&mastergroup);
		core_system->createChannelGroup("BGM Group", &bgmgroup);
		core_system->createChannelGroup("SFX Group", &sfxgroup);

		mastergroup->addGroup(bgmgroup);
		mastergroup->addGroup(sfxgroup);
	}

	Audio_System::Audio_System() : core_system(nullptr), studio_system(nullptr) {
		signature.set(ECSM.get_component_id<Audio_Component>());
		initializegroups();
		if (initialize()) {
			LM.write_log("successfully initialize audio system.");
		}
	}

	Audio_System::~Audio_System() {
		shutdown();
	}

	int Audio_System::errorcheck(FMOD_RESULT result, std::string function_name, std::string function_purpose) {
		if (result != FMOD_OK) {
			LM.write_log("%s failed to %s. FMOD Error: %s.", function_name, function_purpose, FMOD_ErrorString(result));
			return -1;
		}
		LM.write_log("%s successfully executed %s.", function_name, function_purpose);
		return 0;
	}

	bool Audio_System::initialize() {
		FMOD_RESULT result;

		result = FMOD::Studio::System::create(&studio_system);
		if (errorcheck(result) != 0) {
			return false;
		}
		result = studio_system->getCoreSystem(&core_system);
		if (errorcheck(result) != 0) {
			return false;
		}
		result = studio_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_3D_RIGHTHANDED, nullptr);
		if (errorcheck(result) != 0) {
			return false;
		}
		return true;
	}

	void Audio_System::update(float delta_time) {
		const auto& entities = get_entities();
		std::vector<std::string> stopchannel;
		for (EntityID entity : entities) {
			if (!ECSM.has_component<Audio_Component>(entity)) {
				continue;	//need not process if entity has no audio
			}
			Audio_Component& audio = ECSM.get_component<Audio_Component>(entity);
			PlayState state = audio.get_audio_state();
			std::string filepath = audio.get_filename();	//the key for sound
			std::string key_id = filepath + std::to_string(entity);	//the key for channel
			switch (state) {
				case PLAYING:
					play_sound(filepath, key_id);
					std::cout << entity << "sound is playing" << std::endl;
					break;
				case STOPPED:
					stopchannel.push_back(key_id);
					//stop_sound(key_id);
					break;
				case PAUSEDSOUND:
					pause_resume_sound(key_id, true);
					break;
				case RESUMESOUND:
					pause_resume_sound(key_id, false);
				default:
					//nothing to be done? since play state is none
					break;
			}
			std::cout << "this entity is in the audio system " << entity <<"\n";
		}
		for (std::string const& key : stopchannel) {
			stop_sound(key);
		}
		errorcheck(core_system->update());
	}

	void Audio_System::shutdown() {

		for (auto [filepath, sound] : sound_map) {
			unload_sound(filepath);
		}

		//clear the unorderedmap
		sound_map.clear();
		channel_map.clear();
		
		if (core_system) {
			errorcheck(core_system->close());
			errorcheck(core_system->release());
			core_system = nullptr;
		}
		if (studio_system) {
			errorcheck(studio_system->release());
			studio_system = nullptr;
		}
	}


	std::string Audio_System::get_type() const {
		return "Audio_System";
	}

	void Audio_System::load_sound(const std::string& file_path) {
		if (sound_map.find(file_path) != sound_map.end()) {
			//sound is already loaded in the map.
			return;
		}
		FMOD::Sound* sound = nullptr;
		FMOD_MODE mode = FMOD_DEFAULT;
		FMOD_RESULT result = core_system->createSound(file_path.c_str(), mode, 0, &sound);
		if (errorcheck(result,"Audio_System::load_sound():", "create sound") != 0) {
			return;
		}
		std::pair<std::string, FMOD::Sound*> pair = std::make_pair(file_path, sound);
		sound_map.insert(pair);

		LM.write_log("Audio_Sytem::load_sound(): Successfully loaded the sound from system.");
		return;
	}

	void Audio_System::play_sound(const std::string& file_path, std::string& entity_num) {
		if (sound_map.find(file_path) == sound_map.end()) {
			load_sound(file_path);
		}

		FMOD::Channel* channel = nullptr;
		auto sound = sound_map.find(file_path);
		FMOD_RESULT result = core_system->playSound(sound->second, nullptr, false, &channel);
		if (errorcheck(result) != 0) {
			return;
		}
		else {
			std::string key = file_path + entity_num;
			std::pair<std::string, FMOD::Channel*> pair = std::make_pair(key, channel);
			channel_map.insert(pair);
		}
	}

	void Audio_System::pause_resume_sound(const std::string& channel_key, bool pause) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::pause_resume_sound: failed to pause/resume sound as sound isn't even playing in the channel.");
			return;
		}
		auto channel = channel_map.find(channel_key);

		//check pause state of channel
		bool is_currchannel_pause;
		channel->second->getPaused(&is_currchannel_pause);

		//based off the user command if its pause or not.
		if (pause) {
			//if it is pause determine if there is a need to set pause at all based off the current pause state of channel
			//if ispaused is false, means channel is still playing thereafter we need set it to pause
			if (!is_currchannel_pause) {
				errorcheck(channel->second->setPaused(pause));
			}
			else {
				return;
			}
		}
		else {	//if pause is false this means they want to resume the sound if its paused.
			if (is_currchannel_pause) {
				//if current channel is paused and the command is to set it to resume/not pause
				errorcheck(channel->second->setPaused(pause)); //set the pause to be false, aka resume the sound.
			}
			else {
				return;
			}
		}

	}

	void Audio_System::stop_sound(const std::string& channel_key) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::stop_sound: failed to stop sound as sound isn't even playing in the channel.");
			return;
		}
		auto channel = channel_map.find(channel_key);

		bool playstate_currchannel;
		channel->second->isPlaying(&playstate_currchannel);
		if (playstate_currchannel) {
			errorcheck(channel->second->stop()); //if the channel is playing stop it
		}
		else {
			return;
		}
	}

	void Audio_System::unload_sound(const std::string& filepath) {
		if (sound_map.find(filepath) == sound_map.end()) {
			return; //this means sound is not loaded which means no need to unload at all
		}
		auto sound = sound_map.find(filepath);
		FMOD_RESULT result = sound->second->release();	//release the sound
		if (errorcheck(result) != 0) {
			return;
		}
		sound_map.erase(sound);	//erase it from the map
		LM.write_log("Audio_System::unload_sound: successfully unloaded the sound %s", filepath);
	}

	//set and getters for channel & channel group
	void Audio_System::set_channel_pitch(const std::string& channel_key, float pitch) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::set_channel_pitch: failed to set channel pitch as channel is not in channel map.");
			return;
		}
		auto channel = channel_map.find(channel_key);
		errorcheck(channel->second->setPitch(pitch));
	}

	void Audio_System::set_channel_volume(const std::string& channel_key, float volume) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::set_channel_volume: failed to set channel volume as channel is not in channel map.");
			return;
		}
		auto channel = channel_map.find(channel_key);
		errorcheck(channel->second->setVolume(volume));
	}

	void Audio_System::pause_bgm_group() {
		bool pause;
		bgmgroup->getPaused(&pause);
		if (pause) {
			return;
		}
		else {
			errorcheck(bgmgroup->setPaused(true));
		}
	}
	void Audio_System::resume_bgm_group() {
		bool pause;
		bgmgroup->getPaused(&pause);
		if (pause) {
			errorcheck(bgmgroup->setPaused(false));
		}
		else {
			return;
		}
	}

	void Audio_System::pause_sfx_group() {
		bool pause;
		sfxgroup->getPaused(&pause);
		if (pause) {
			return;
		}
		else {
			errorcheck(sfxgroup->setPaused(true));
		}
	}
	void Audio_System::resume_sfx_group() {
		bool pause;
		sfxgroup->getPaused(&pause);
		if (pause) {
			errorcheck(sfxgroup->setPaused(false));
		}
		else {
			return;
		}
	}

	float Audio_System::get_bgmgroup_volume() const {
		float volume;
		bgmgroup->getVolume(&volume);
		return volume;
	}

	void Audio_System::set_bgmgroup_volume(float volume) {
		errorcheck(bgmgroup->setVolume(volume));
	}

	float Audio_System::get_bgmgroup_pitch() const {
		float pitch;
		bgmgroup->getPitch(&pitch);
		return pitch;
	}

	void Audio_System::set_bgmgroup_pitch(float pitch) {
		errorcheck(bgmgroup->setPitch(pitch));
	}

	float Audio_System::get_sfxgroup_volume() const {
		float volume;
		sfxgroup->getVolume(&volume);
		return volume;
	}

	void Audio_System::set_sfxgroup_volume(float volume) {
		errorcheck(sfxgroup->setVolume(volume));
	}

	float Audio_System::get_sfxgroup_pitch() const {
		float pitch;
		sfxgroup->getPitch(&pitch);
		return pitch;
	}

	void Audio_System::set_sfxgroup_pitch(float pitch) {
		errorcheck(sfxgroup->setPitch(pitch));
	}

	float Audio_System::get_mastergroup_volume() const {
		float volume;
		mastergroup->getVolume(&volume);
		return volume;
	}

	void Audio_System::set_mastergroup_volume(float volume) {
		errorcheck(mastergroup->setVolume(volume));
	}

	float Audio_System::get_mastergroup_pitch() const {
		float pitch;
		mastergroup->getPitch(&pitch);
		return pitch;
	}

	void Audio_System::set_mastergroup_pitch(float pitch) {
		errorcheck(mastergroup->setPitch(pitch));
	}

	FMOD::ChannelGroup* Audio_System::get_bgmgroup() {
		return bgmgroup;
	}

	FMOD::ChannelGroup* Audio_System::get_sfxgroup() {
		return sfxgroup;
	}

	FMOD::ChannelGroup* Audio_System::get_mastergroup() {
		return mastergroup;
	}
}

	