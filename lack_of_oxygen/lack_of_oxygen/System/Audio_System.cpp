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
#include "../Manager/Assets_Manager.h"

namespace lof {

	//initialize all the group
	void Audio_System::initializegroups() {
		errorcheck(core_system->getMasterChannelGroup(&mastergroup), "Audio_System::initializegroups()", "initializing mastergroup");
		core_system->createChannelGroup("BGM Group", &bgmgroup);
		core_system->createChannelGroup("SFX Group", &sfxgroup);

		mastergroup->addGroup(bgmgroup);
		mastergroup->addGroup(sfxgroup);
	}

	//initialize the core system
	Audio_System::Audio_System() : core_system(nullptr), mastergroup(nullptr), bgmgroup(nullptr), sfxgroup(nullptr) {
		signature.set(ECSM.get_component_id<Audio_Component>());	//initialize the signature set for the audio component
		//initializegroups();
		if (initialize()) {
			LM.write_log("successfully initialize audio system.");
		}
	}

	//destructor for the audio system
	Audio_System::~Audio_System() {
		shutdown();
	}

	//function to check for error
	int Audio_System::errorcheck(FMOD_RESULT result, const std::string& function_name, const std::string& function_purpose) {
		if (result != FMOD_OK) {
			LM.write_log("%s failed to %s. FMOD Error: %s.", function_name.c_str(), function_purpose.c_str(), FMOD_ErrorString(result));
			return -1;
		}
		LM.write_log("%s successfully executed %s.", function_name.c_str(), function_purpose.c_str());
		return 0;
	}

	//initializes the audio system
	bool Audio_System::initialize() {
		FMOD_RESULT result;

		result = FMOD::System_Create(&core_system);
		if (errorcheck(result, "Audio_System::initialize", "create core system") != 0) {
			return false;
		}

		result = core_system->init(512, FMOD_INIT_NORMAL, 0);
		if (errorcheck(result, "Audio_System::initialize", "initialize studio system") != 0) {
			return false;
		}

		initializegroups();

		return true;
	}

	void Audio_System::update(float delta_time) {
		(void)delta_time;
		const auto& entityids = get_entities();
		std::vector<std::string> stopchannel;
		for (EntityID entityID : entityids) {
			if (!ECSM.has_component<Audio_Component>(entityID)) {
				continue;	//need not process if entity has no audio
			}
			Audio_Component& audio = ECSM.get_component<Audio_Component>(entityID);
			
			const auto& sounds = audio.get_sounds();

			for (const auto& sound : sounds) {
				std::string audio_key = sound.key;
				std::string key_id = audio.get_filepath(audio_key) + std::to_string(entityID);
				PlayState state = audio.get_audio_state(audio_key);

				switch (state) {
				case PLAYING:
					play_sound(audio.get_filepath(audio_key), key_id, audio_key, audio);
					break;
				case STOPPED:
					stop_sound(key_id);
					stopchannel.push_back(key_id);
					break;
				case PAUSED:
					pause_resume_sound(key_id, true);
					break;
				case RESUMED:
					pause_resume_sound(key_id, false);
					break;
				default:
					stopchannel.push_back(key_id);
					continue;
				}

				if (channel_map.find(key_id) != channel_map.end()) {
					float currvolume, currpitch;

					if (channel_map[key_id]->getVolume(&currvolume) != audio.get_volume(audio_key)) {
						set_channel_volume(key_id, audio.get_volume(audio_key));
					}

					if (channel_map[key_id]->getPitch(&currpitch) != audio.get_pitch(audio_key)) {
						set_channel_pitch(key_id, audio.get_pitch(audio_key));
					}
				}
			}
			
		}
		//free up unused channel.
		for (std::string const& key : stopchannel) {
			channel_map.erase(key);
		}

		//update the fmod system with the core system
		errorcheck(core_system->update(), "Audio_System::update", "update core system");
	}

	void Audio_System::shutdown() {

		stop_mastergroup();

		std::vector<std::string> to_unload;

		for (const auto& sound : sound_map) {
			to_unload.push_back(sound.first);
		}

		for (const auto& filepath : to_unload) {
			unload_sound(filepath);
		}

		//clear the unorderedmap
		sound_map.clear();
		channel_map.clear();
		
		if (core_system) {
			//errorcheck(core_system->close(), "Audio_System::shutdown", "close core system");
			errorcheck(core_system->release(), "Audio_System::shutdown", "release core system");
			core_system = nullptr;
		}

		LM.write_log("Audio System shutdown successfully");
	}


	std::string Audio_System::get_type() const {
		return "Audio_System";
	}

#if 0
	void Audio_System::load_sound(const std::string& file_path) {
		if (sound_map.find(file_path) != sound_map.end()) {
			//sound is already loaded in the map.
			return;
		}


		LM.write_log("The file path for load_sound is: %s", file_path.c_str());
		FMOD::Sound* sound = nullptr;
		FMOD_MODE mode = FMOD_DEFAULT;
		FMOD_RESULT result = core_system->createSound(file_path.c_str(), mode, 0, &sound);
		if (errorcheck(result,"Audio_System::load_sound():", "create sound") != 0) {
			return;
		}
		std::pair<std::string, FMOD::Sound*> pair = std::make_pair(file_path, sound);
		sound_map.insert(pair);

		LM.write_log("Audio_System::load_sound(): Successfully loaded the sound from system.");
		return;
	}
	#endif

	void Audio_System::play_sound(const std::string& file_path, std::string& cskey, std::string& audio_key, Audio_Component& audio) {
		//check if sound has already been loaded.
		if (sound_map.find(file_path) == sound_map.end()) {
			load_sound(file_path);
			if (sound_map.find(file_path) == sound_map.end()) {
				LM.write_log("audiosystem play_sound fail to load sound %s.", file_path.c_str());
				return;
			}
		}

		//check if the sound is already playing under the entity in a channel
		if ((channel_map.find(cskey) != channel_map.end()) && channel_map.find(cskey)->second != nullptr) {
			//check if sound has finish playing, if so change the audio_state to stopped.
			bool playing = false;
			errorcheck(channel_map.find(cskey)->second->isPlaying(&playing), "Audio_System::play_sound", "check if channel is playing");
			if (!playing) {
				audio.set_audio_state(audio_key, NONE);
				channel_map.erase(cskey);
			}
			return;	//sound is already playing
		}

		//find the loaded sound file
		FMOD::Channel* channel = nullptr;
		//FMOD::ChannelGroup* group = nullptr;
		auto sound = sound_map.find(file_path);

		FMOD_RESULT result = core_system->playSound(sound->second, nullptr, false, &channel);
		if (errorcheck(result, "Audio_System::play_sound", "play sound" + file_path) != 0 || !channel) {
			LM.write_log("Audio_System::play_sound: Channel creation failed for %s", file_path.c_str());
			return;
		}
		else {
			channel_map[cskey] = channel;
		}

		//add channel into the respective channel group
		if (audio.get_audio_type(audio_key) == BGM) {
			LM.write_log("audio added into BGM GROUP");
			channel->setChannelGroup(bgmgroup);
		}
		else {
			channel->setChannelGroup(sfxgroup);
			LM.write_log("audio added into SFX Group");
		}

		if (audio.get_loop(audio_key)) {
			channel->setMode(FMOD_LOOP_NORMAL);
			channel->setLoopCount(-1);	//<-1 for indefinite playing of sound in channel
		}
		else {
			channel->setLoopCount(0);	//set it to 0 to play sound once.
		}

		//set channel pitch and volume
		channel->setPitch(audio.get_pitch(audio_key));
		channel->setVolume(audio.get_volume(audio_key));

		LM.write_log("Audio_System::play_sound: sound %s is playing", cskey.c_str());
	}

	void Audio_System::load_sound(const std::string& file_path) {
		if (sound_map.find(file_path) != sound_map.end()) {
			return; // Sound already loaded
		}

		// Use Assets Manager to get path and verify file
		if (!ASM.load_audio_file(file_path)) {
			LM.write_log("Audio_System::load_sound: Failed to find audio file %s", file_path.c_str());
			return;
		}

		std::string full_path = ASM.get_audio_path(file_path);
		LM.write_log("Audio_System::load_sound: Loading sound from %s", full_path.c_str());

		FMOD::Sound* sound = nullptr;
		FMOD_MODE mode = FMOD_DEFAULT;
		FMOD_RESULT result = core_system->createSound(full_path.c_str(), mode, 0, &sound);
		if (errorcheck(result, "Audio_System::load_sound", "create sound") != 0) {
			return;
		}

		sound_map[file_path] = sound;
		LM.write_log("Audio_System::load_sound: Successfully loaded sound");
	}


	void Audio_System::stop_mastergroup() {
		bool playing;
		mastergroup->isPlaying(&playing);
		if (playing) {
			errorcheck(mastergroup->stop(), "Audio_System::stop_mastergroup", "stop master group");
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
				errorcheck(channel->second->setPaused(pause), "Audio_System::pause_resume_sound", "pause the channel");
			}
			else {
				return;
			}
		}
		else {	//if pause is false this means they want to resume the sound if its paused.
			if (is_currchannel_pause) {
				//if current channel is paused and the command is to set it to resume/not pause
				errorcheck(channel->second->setPaused(pause), "Audio_System::pause_resume_sound", "resume sound"); //set the pause to be pause(false), aka resume the sound.
			}
			else {
				return;
			}
		}

	}

	void Audio_System::stop_sound(const std::string& channel_key) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::stop_sound: failed to stop sound as %s isn't even playing in the channel.", channel_key.c_str());
			return;
		}
		auto channel = channel_map.find(channel_key);

		bool playstate_currchannel;
		channel->second->isPlaying(&playstate_currchannel);
		if (playstate_currchannel) {
			errorcheck(channel->second->stop(), "Audio_System::stop_sound", "stop channel" + channel_key); //if the channel is playing stop it
		}
		else {
			return;	//nothing to do as channel has already finish playing music
		}
	}

	//not sure if i am keeping this message
	void Audio_System::unload_sound(const std::string& filepath) {
		if (sound_map.find(filepath) == sound_map.end()) {
			return; //this means sound is not loaded which means no need to unload at all
		}
		auto sound = sound_map.find(filepath);
		FMOD::Sound* sounds = sound->second;
		FMOD_RESULT result = sounds->release();
		if (errorcheck(result, "Audio_System::unload_sound", "release sound" + filepath) != 0) {
			return;
		}
		sound_map.erase(sound);	//erase it from the map
		LM.write_log("Audio_System::unload_sound: successfully unloaded the sound %s", filepath.c_str());
	}

	//set and getters for channel & channel group
	void Audio_System::set_channel_pitch(const std::string& channel_key, float pitch) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::set_channel_pitch: failed to set channel pitch as channel is not in channel map.");
			return;
		}
		auto channel = channel_map.find(channel_key);
		errorcheck(channel->second->setPitch(pitch), "Audio_System::set_channel_pitch", "set pitch for channel: " + channel_key);
	}

	void Audio_System::set_channel_volume(const std::string& channel_key, float volume) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::set_channel_volume: failed to set channel volume as channel is not in channel map.");
			return;
		}
		auto channel = channel_map.find(channel_key);
		errorcheck(channel->second->setVolume(volume), "Audio_System::set_channel_volume", "set volume for channel: " + channel_key);
	}

	void Audio_System::pause_bgm_group() {
		bool pause;
		bgmgroup->getPaused(&pause);
		if (pause) {
			return;
		}
		else {
			errorcheck(bgmgroup->setPaused(true), "Audio_System::pause_bgm_group", "pause bgmgroup");
		}
	}
	void Audio_System::resume_bgm_group() {
		bool pause;
		bgmgroup->getPaused(&pause);
		if (pause) {
			errorcheck(bgmgroup->setPaused(false), "Audio_System::resume_bgm_group", "unpause bgmgroup");
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
			errorcheck(sfxgroup->setPaused(true), "Audio_System::pause_sfx_group", "pause sfxgroup");
		}
	}
	void Audio_System::resume_sfx_group() {
		bool pause;
		sfxgroup->getPaused(&pause);
		if (pause) {
			errorcheck(sfxgroup->setPaused(false), "Audio_System::resume_sfx_group", "unpause sfxgroup");
		}
		else {
			return;
		}
	}

	void Audio_System::pause_resume_mastergroup() {
		//get_mastergroup;
		bool pause;
		mastergroup->getPaused(&pause);
		if (pause) {
			errorcheck(mastergroup->setPaused(false), "Audio_System::pause_resume_group", "resume mastergroup");
		}
		else {
			errorcheck(mastergroup->setPaused(true), "Audio_System::pause_resume_group", "pause mastergroup");
		}
		
	}

	float Audio_System::get_bgmgroup_volume() const {
		float volume;
		bgmgroup->getVolume(&volume);
		return volume;
	}

	void Audio_System::set_bgmgroup_volume(float volume) {
		errorcheck(bgmgroup->setVolume(volume), "Audio_System::set_bgmgroup_volume", "set bgmgroup volume");
	}

	float Audio_System::get_bgmgroup_pitch() const {
		float pitch;
		bgmgroup->getPitch(&pitch);
		return pitch;
	}

	void Audio_System::set_bgmgroup_pitch(float pitch) {
		errorcheck(bgmgroup->setPitch(pitch), "Audio_System::set_bgmgroup_pitch", "set bgmgroup pitch");
	}

	float Audio_System::get_sfxgroup_volume() const {
		float volume;
		sfxgroup->getVolume(&volume);
		return volume;
	}

	void Audio_System::set_sfxgroup_volume(float volume) {
		errorcheck(sfxgroup->setVolume(volume), "Audio_System::set_sfxgroup_volume", "set sfxgroup volume");
	}

	float Audio_System::get_sfxgroup_pitch() const {
		float pitch;
		sfxgroup->getPitch(&pitch);
		return pitch;
	}

	void Audio_System::set_sfxgroup_pitch(float pitch) {
		errorcheck(sfxgroup->setPitch(pitch), "Audio_System::set_sfxgroup_pitch", "set sfxgroup pitch");
	}

	float Audio_System::get_mastergroup_volume() const {
		float volume;
		mastergroup->getVolume(&volume);
		return volume;
	}

	void Audio_System::set_mastergroup_volume(float volume) {
		errorcheck(mastergroup->setVolume(volume), "Audio_System::set_mastergroup_volume", "set mastergroup volume");
	}

	float Audio_System::get_mastergroup_pitch() const {
		float pitch;
		mastergroup->getPitch(&pitch);
		return pitch;
	}

	void Audio_System::set_mastergroup_pitch(float pitch) {
		errorcheck(mastergroup->setPitch(pitch), "Audio_System::set_mastergroup_pitch", "set mastergroup pitch");
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

	