/**
 * @file Audio_System.cpp
 * @brief Define of the Audio_System class for managing audio playback using FMOD.
 * @author Amanda Leow Boon Suan (98%), Saw Hui Shan (2%)
 * @date created September 27, 2024 
 * @date updated February 3, 2025
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "Audio_System.h"

namespace lof {
	Audio_System::Audio_System() : core_system(nullptr) {
		signature.set(ECSM.get_component_id<Audio_Component>());	//initialize the signature set for the audio component
		signature.set(ECSM.get_component_id<Transform2D>());	//this is for the listener, etc

		if (initialize()) {
			LM.write_log("successfully initialize audio system.");
		}
	}

	Audio_System::~Audio_System() {
		shutdown();
	}

	bool Audio_System::initialize() {
		FMOD_RESULT result;

		result = FMOD::System_Create(&core_system);
		if (ADM.errorcheck(result, "Audio_System::initialize", "create core system") != 0) {
			return false;
		}

		result = core_system->init(512, FMOD_INIT_NORMAL, 0);
		if (ADM.errorcheck(result, "Audio_System::initialize", "initialize studio system") != 0) {
			return false;
		}

		return true;
	}
	void Audio_System::update(float delta_time) {

		(void)delta_time;
		const auto& entityids = get_entities();

		std::vector<std::string> channels_to_remove;

		for (EntityID entityID : entityids) {
			if (!ECSM.has_component<Audio_Component>(entityID)) {
				continue;
			}

			Audio_Component& audio = ECSM.get_component<Audio_Component>(entityID);
			const auto& sounds = audio.get_sounds();

			if (entityID == ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME)) {
				if (!ECSM.has_component<Transform2D>(entityID)) {
					LM.write_log("Audio_System. update has detected player to update listener_pos but no transform component");
					continue;
				}
				//update listener position here
				Transform2D& transform = ECSM.get_component<Transform2D>(entityID);
				update_audio_for_listener(vec2d_to_vec3d(transform.position));
			}

			//loop through the sound struct in each entity
			for (const auto& sound : sounds) {
				std::string audio_key = sound.key;
				std::string file_path = audio.get_filepath(audio_key);
				std::string channel_key = file_path + std::to_string(entityID) + audio_key;

				//check if the file exist, if it no longer does stop and release the sound immediately if it is still playing
				if (!ASM.load_audio_file(audio.get_filepath(audio_key))) {
					LM.write_log("Audio_System::update Audio File %s no longer exist", audio.get_filepath(audio_key).c_str());
					if (channel_map.find(channel_key) != channel_map.end()) {
						stop_sound(channel_key);
						ADM.unload_sound(audio.get_filepath(audio_key));
					}
					continue;
				}

				// essentially ensure when sound is loaded it gets checked. if its not loaded then skip the check
				//check if the filepath for the specific sound/audio key has been changed
				auto it1 = all_prev_filepath_map.find(audio_key);
				//if it cannot be find means it have yet to be initialize in the filepath and its the first instance of it
				if (it1 == all_prev_filepath_map.end()) {
					all_prev_filepath_map[audio_key] = file_path;
				}
				else {
					//checking if filepath name aligns
					if (it1->second != file_path) {
						LM.write_log("Audio_System::play_sound: Stopping previous sound %s due to audio key %s path is mismatch", it1->second.c_str(), audio_key.c_str());
				
						std::string old_channel_key = it1->second + std::to_string(entityID) + audio_key;
				
						stop_sound(old_channel_key);
						it1->second = file_path; //ensure to keep the map updated
					}
				}

				if (channel_map.find(channel_key) == channel_map.end()) {
					//channel not found, meaning sound is not playing
					continue;
				}

				std::vector<FMOD::Channel*> to_remove;

				//to retrieve the vector of channels in channel map data
				auto& channels = channel_map.find(channel_key)->second;

				//loop through to check if channel is playing. if it is update if needed and if it isn't add it to the remove vector
				for (auto it2 = channels.begin(); it2 != channels.end(); it2++) {
					bool is_playing = false;
					ADM.errorcheck((*it2)->isPlaying(&is_playing), "Audio_System::update", "check if channel is playing");

					if (!is_playing) {
						to_remove.push_back(*it2);
					}
				}

				//remove the stopped channels in the vector of channels in channel_map data
				for (auto& channel : to_remove) {
					auto it = std::find(channels.begin(), channels.end(), channel);
					if (it != channels.end()) {
						channels.erase(it);
					}
				}

				//after the removal of the channels in the channel vector check if the channel is empty if so add it to the remove channel list
				if (channels.empty()) {
					channels_to_remove.push_back(channel_key);
				}


				//check if need to add a function to control bgm playback so it is easier

				//add any other function here that is for sound in each entity here
			}
		}

		//cleanup finished channels from map
		for (const auto& key : channels_to_remove) {
			//check if sound is still playing inside if it isn't return if it is stop and thereafter remove from channel map
			stop_sound(key);

			//to clean up should it not have been done so in the stop_sound(key) function
			if (channel_map.find(key) != channel_map.end()) {
				channel_map.erase(key);
			}
		}

		

		core_system->update();

		//to be implemented later
		// 
		// things to consider
		// seek help from prof elie if can't solve by today.

		//THIS IS FOR DEBUG PURPOSE TO BE COMMENTED OUT IF NOT NEEDED (WILL OVERLOAD QUITE ABIT AS IT CHECKS FOR ACTIVE CHANNELS EVERY LOOP)
		//get_active_channels();
		//get_muted_channels();
	}

	void Audio_System::shutdown() {

		//technically speaking as sound/channel or stop_mastergroup has been called in ADM shutdown

		channel_map.clear();
		all_prev_filepath_map.clear();

		if (core_system) {
			//errorcheck(core_system->close(), "Audio_System::shutdown", "close core system");
			ADM.errorcheck(core_system->release(), "Audio_System::shutdown", "release core system");
			core_system = nullptr;
		}

		LM.write_log("Audio System shutdown successfully");
	}

	std::string Audio_System::generate_channel_key(EntityID entity_id, const std::string& file_path, const std::string& audio_key) {
		return file_path + std::to_string(entity_id) + audio_key;
	}

	void Audio_System::play_sfx_sound(const std::string& file_path, std::string& cskey, const std::string& audio_key, const Audio_Component& audio) {

		auto it1 = all_prev_filepath_map.find(audio_key);
		if (it1 == all_prev_filepath_map.end()) {
			all_prev_filepath_map[audio_key] = file_path;
		}
		else {
			if (it1->second != file_path) {
				LM.write_log("Audio_System::play_sound: Stopping previous sound %s due to audio key %s path is mismatch", it1->second.c_str(), audio_key.c_str());

				std::string entityID = cskey.substr(file_path.length(), cskey.length() - file_path.length() - audio_key.length());

				std::string old_key_id = it1->second + entityID + audio_key;

				stop_sound(old_key_id);

				it1->second = file_path; //update the file_path.
			}
		}

		auto it2 = channel_map.find(cskey);

		//check if the channel already exist in the map
		if (it2 != channel_map.end() && !it2->second.empty()) {
			//only check if the size of the vector is at max as well as its the min simultaneous aka 1.
			//sounds affected: walking, airvent in, (to be added on)
			if (it2->second.size() == audio.get_max_simultaneous(audio_key) && audio.get_max_simultaneous(audio_key) == MIN_SIMULTANEOUS) {
				FMOD::Channel* existing_channel = it2->second.front();

				bool playing = false;
				existing_channel->isPlaying(&playing);
				if (playing) {
					int get_current_loop_count;
					existing_channel->getLoopCount(&get_current_loop_count);
					existing_channel->setLoopCount(get_current_loop_count + 1);
					LM.write_log("Audio_System::play_sfx_sound: Loop count increased for %s", cskey.c_str());
					return;
				}
				else {
					//if it is not playing but channel still in the vector, remove (erase) it from the vector so it can be played again alter on
					it2->second.erase(it2->second.begin());
				}		
			}
		}

		//BY HERE USUALLY ITS EITHER 1. ITS A SOUND THAT CAN STACK/PLAY MULTIPLE SIMULTANEOUSLY 2. ITS NOT A SOUND FOR SIMULTANEOUS PLAYING BUT HAS ALREADY STOPPED PLAYING AND IS NO LONGER IN THE MAP.

		FMOD::Sound* sound = ADM.get_sound(file_path, audio.get_audio_type(audio_key), audio.get_is3d(audio_key));

		auto& channels = channel_map[cskey];
		//it checks if the max channel size has been reach and if so it stops the first one and play the next <- this is for mining specially tbh.
		//it needs to check incase the number of channel currently is less than 1. (if so no need stop) //THINK AGAIN ABOUT THIS LOGIC
		if (channels.size() >= audio.get_max_simultaneous(audio_key) && audio.get_max_simultaneous(audio_key) >= MIN_SIMULTANEOUS) {
			LM.write_log("Max simultaneous channel reached, stopping the first channel");
			FMOD::Channel* first = channels.front();
			first->stop();
			channels.erase(channels.begin());
		}

		if (audio.get_is3d(audio_key)) {
			FMOD_MODE mode;
			sound->getMode(&mode);
			LM.write_log("mode currently is initially: %s", ADM.modeToString(mode).c_str());
			// Desired mode should have FMOD_3D and FMOD_3D_INVERSEROLLOFF flags
			//FMOD_MODE desiredMode = FMOD_DEFAULT | FMOD_3D | FMOD_3D_INVERSEROLLOFF;

			//// Check if the 3D flag is set (ignore other flags for now)
			//if (!(mode & FMOD_3D)) {
			//	LM.write_log("Setting mode to 3D (with inverse rolloff)...");
			//	sound->setMode(desiredMode);
			//}
			//else {
			//	LM.write_log("Mode is already 3D: %s", ADM.modeToString(mode).c_str());
			//}

			//// Verify that the mode is set correctly after applying
			//sound->getMode(&mode);
			//LM.write_log("Mode after setting: %s", ADM.modeToString(mode).c_str());
		}

		FMOD::Channel* channel = nullptr;

		FMOD_RESULT result = core_system->playSound(sound, nullptr, false, &channel);
		if (ADM.errorcheck(result) != 0 || !channel) {
			LM.write_log("Audio_System::play_sound: Channel creation failed for %s", file_path.c_str());
			return;
		}

		channels.push_back(channel);

		//add channel into the respective channel group
		if (audio.get_audio_type(audio_key) == SFX) {
			channel->setChannelGroup(ADM.get_sfxgroup());
			LM.write_log("audio added into SFX Group");
		}
		else {
			channel->setChannelGroup(ADM.get_uigroup());
			LM.write_log("audio added into UI Group");
		}

		if (audio.get_loop(audio_key)) {
			channel->setMode(FMOD_LOOP_NORMAL);
			channel->setLoopCount(-1);	//<-1 for indefinite playing of sound in channel
		}
		else {
			channel->setLoopCount(0);	//set it to 0 to play sound once.
		}

		if (audio.get_is3d(audio_key)) {
			set_sound_position(cskey, audio.get_position());
		}

		//set channel pitch and volume
		channel->setPitch(audio.get_pitch(audio_key));
		channel->setVolume(audio.get_volume(audio_key));

		LM.write_log("Audio_System::play_sound: sound %s is playing ", cskey.c_str());
	}

	void Audio_System::play_bgm_sound(const std::string& file_path, std::string& cskey, const std::string& audio_key, const Audio_Component& audio) {
		
		auto it2 = channel_map.find(cskey);
		if (it2 != channel_map.end() && !it2->second.empty()) {

			FMOD::Channel* existing_channel = it2->second.front();
			bool is_playing = false;

			existing_channel->isPlaying(&is_playing);
			if (is_playing) {
				LM.write_log("BGM %s is already playing, skipping restart.", cskey.c_str());
				return;
			}
			else {
				LM.write_log("BGM %s was in channel_map but is NOT playing, restarting.", cskey.c_str());
				stop_sound(cskey);
			}

			LM.write_log("BGM %s is already playing", cskey.c_str());
			return;
		}

		//check has the sound filepath has been changed
		auto it1 = all_prev_filepath_map.find(audio_key);
		if (it1 == all_prev_filepath_map.end()) {
			all_prev_filepath_map[audio_key] = file_path;
		}
		else {
			if (it1->second != file_path) {
				LM.write_log("Audio_System::play_sound: Stopping previous sound %s due to audio key %s path is mismatch", it1->second.c_str(), audio_key.c_str());

				std::string entityID = cskey.substr(file_path.length(), cskey.length() - file_path.length() - audio_key.length());

				std::string old_key_id = it1->second + entityID + audio_key;

				stop_sound(old_key_id);

				it1->second = file_path; //update the file_path.
			}
		}

		//debug_list_active_sounds();

		auto& channels = channel_map[cskey];
		if (channels.empty() && channels.size() <= audio.get_max_simultaneous(audio_key)) {
			//check if sound exist in soundmap
			FMOD::Sound* sound = ADM.get_sound(file_path, audio.get_audio_type(audio_key), audio.get_is3d(audio_key));
			//sound->setMode(FMOD_2D);
			FMOD::Channel* channel = nullptr;

			FMOD_RESULT result = core_system->playSound(sound, nullptr, false, &channel);
			if (ADM.errorcheck(result) != 0 || !channel) {
				LM.write_log("Audio_System::play_sound: Channel creation failed for %s", file_path.c_str());
				return;
			}

			channels.push_back(channel);

			channel->setChannelGroup(ADM.get_bgmgroup());

			if (audio.get_loop(audio_key)) {
				channel->setMode(FMOD_LOOP_NORMAL);
				channel->setLoopCount(-1);	//<-1 for indefinite playing of sound in channel
			}
			else {
				//by right bgm should be looping but just incase
				channel->setLoopCount(0);	//set it to 0 to play sound once.
			}

			//set channel pitch and volume
			channel->setPitch(audio.get_pitch(audio_key));
			channel->setVolume(audio.get_volume(audio_key));

			std::cout << "bgm is playing at " << audio.get_volume(audio_key) << std::endl;

			//debug_list_active_sounds();
		}
		else {
			return;	//sound already is playing need not play anymore
		}
	}

	//need reconsider how will we be using this function
	void Audio_System::pause_resume_sound(const std::string& channel_key, bool pause) {
		auto it = channel_map.find(channel_key);
		if (it == channel_map.end()) {
			LM.write_log("Audio_System::pause_resume_sound: failed to pause/resume sound as sound isn't even playing in the channel.");
			return;
		}

		//check pause state of channel
		bool is_currchannel_pause = false;

		auto channels = it->second;

		for (auto channel : channels) {
			channel->getPaused(&is_currchannel_pause);

			//based off the user command if its pause or not.
			if (pause && !is_currchannel_pause) {
				//if pause command is detected from user input and current channel is not pause, pause the channel
				ADM.errorcheck(channel->setPaused(pause), "Audio_System::pause_resume_sound", "pause the channel");
			}
			else if (!pause && is_currchannel_pause) {	//if pause is false this means they want to resume the sound if its paused.
				ADM.errorcheck(channel->setPaused(pause), "Audio_System::pause_resume_sound", "resume sound"); //set the pause to be pause(false), aka resume the sound.
			}
		}

	}

	void Audio_System::stop_sound(const std::string& channel_key) {

		//check if the channel key even exist in the map
		auto it = channel_map.find(channel_key);
		if (it == channel_map.end()) {
			LM.write_log("Audio_System::stop_sound: %s isn't found in the channelmap.", channel_key.c_str());
			return;
		}

		auto& channels = it->second;

		for (FMOD::Channel* channel : channels) {
			if (channel == nullptr) {
				LM.write_log("Audio_System::stop_sound: failed to stop sound as %s is a nullptr.", channel_key.c_str());
				return;
			}

			bool playstate_currchannel = false;
			ADM.errorcheck(channel->isPlaying(&playstate_currchannel), "Audio_System::stop_sound", "check sound playing");
			if (playstate_currchannel) {
				ADM.errorcheck(channel->stop(), "Audio_System::stop_sound", "stop channel" + channel_key); //if the channel is playing stop it
			}
			else {
				continue;	//nothing to do as channel has already finish playing music

			}
		}
		channels.clear(); //clear the vector as no channels should remain after stopping

		//technically once function reaches here it means all channel in the channel key data vector has been stopped
		//so removal shouldn't cause an issues
		channel_map.erase(channel_key);
	}

	void Audio_System::set_channel_pitch(const std::string& channel_key, float pitch) {
		if (channel_map.find(channel_key) == channel_map.end()) {
			LM.write_log("Audio_System::set_channel_pitch: failed to set channel pitch as channel is not in channel map.");
			return;
		}
		auto it = channel_map.find(channel_key);
		auto channels = it->second;

		float ori_pitch = 0.0f;

		for (FMOD::Channel* channel : channels) {
			channel->getPitch(&ori_pitch);
			if (ori_pitch == pitch) {
				continue;
			}
			ADM.errorcheck(channel->setPitch(pitch), "Audio_System::set_channel_pitch", "set pitch for channel: " + channel_key);
		}
	}

	void Audio_System::set_channel_volume(const std::string& channel_key, float volume) {
		
		auto it = channel_map.find(channel_key);
		
		if (it == channel_map.end()) {
			LM.write_log("Audio_System::set_channel_volume: failed to set channel volume as channel is not in channel map.");
			return;
		}

		auto channels = it->second;

		float ori_volume = 0.0f;

		for (FMOD::Channel* channel : channels) {
			channel->getVolume(&ori_volume);
			if (ori_volume == volume) {
				continue;
			}
			ADM.errorcheck(channel->setVolume(volume), "Audio_System::set_channel_volume", "set volume for channel: " + channel_key);
		}
	}

	void Audio_System::set_channel_mute(const std::string& channel_key, bool mute) {
		auto it = channel_map.find(channel_key);
		
		if (it == channel_map.end()) {
			LM.write_log("Audio_System::set_channel_mute: failed to set channel mute as channel is not in channel map.");
			return;
		}

		auto channels = it->second;

		bool muted = false;

		for (FMOD::Channel* channel : channels) {
			channel->getMute(&muted);
			if (mute) {
				if (!muted) {
					channel->setMute(true);
				}
				else {
					continue;
				}
			}
			else {
				if (!muted) {
					continue;
				}
				else {
					channel->setMute(false);
				}
			}
		}
	}

	void Audio_System::get_channel_mute(const std::string& channel_key, bool& muted) {
		auto it = channel_map.find(channel_key);

		if (it == channel_map.end()) {
			LM.write_log("Audio_System::get_channel_mute: failed to get mute status of channel as channel is not in the map");
			return;
		}

		auto channels = it->second;
		FMOD::Channel* channel = it->second.front();
		channel->getMute(&muted);

		return;
	}

	FMOD::System* Audio_System::get_core_system() {
		LM.write_log("Audio_System::get_core_system: retrieveing core_system");
		return core_system;
	}

	Vec3D Audio_System::get_channel_pos(const std::string& channel_key) {
		auto it = channel_map.find(channel_key);
		if (it == channel_map.end()) {
			return Vec3D(); //there is no position by right if channel doesn't exist for now standard is return as default.
		}

		FMOD_VECTOR pos;
		for (FMOD::Channel* channel : it->second) {
			channel->get3DAttributes(&pos, nullptr);
			return Vec3D(pos.x, pos.y, pos.z);
		}

		return Vec3D();
	}

	Vec3D Audio_System::get_listener_pos() {
		FMOD_VECTOR pos;
		core_system->get3DListenerAttributes(0, &pos, nullptr, nullptr, nullptr);
		return Vec3D(pos.x, pos.y, pos.z);
	}

	void Audio_System::update_audio_for_listener(const Vec3D& listener_pos) {
		FMOD_VECTOR fmod_listener_pos = { listener_pos.x, listener_pos.y, listener_pos.z };
		FMOD_VECTOR fmod_forward = {0.0f, 0.0f, 1.0f }; //listener facing along x-axis
		FMOD_VECTOR fmod_up = { 0.0f, 1.0f, 0.0f }; // listener is up along the y-axis

		ADM.errorcheck(core_system->set3DListenerAttributes(
			0,						//First (and only) listener
			&fmod_listener_pos,		//Position
			nullptr,				//Velocity (this is for dopller which we do not need)
			&fmod_forward,			//Forward orientation
			&fmod_up),				//Up orientation
			"Audio_System::update_audio_for_listener", "set listener attributes");


		//std::cout << "listener_pos.x: " << listener_pos.x << " listener_pos.y: " << listener_pos.y << " listener_pos.z: " << listener_pos.z << std::endl;
	}

	void Audio_System::set_sound_position(const std::string& channel_key, const Vec3D position) {
		auto it = channel_map.find(channel_key);
		if (it == channel_map.end()) {
			return; //if the sound is not even playing in any channel there is no position to set
		}

		FMOD_VECTOR pos = { position.x, position.y, position.z };

		auto channels = it->second;

		for (FMOD::Channel* channel : channels) {
			if (channel) {
				channel->set3DAttributes(&pos, nullptr);

				//std::cout << "sound pos x: " << pos.x << " pos. y: " << pos.y << "pos.x: " << pos.x << std::endl;

				channel->set3DMinMaxDistance(10.0f, 100.0f); //TO BE ADJUSTED MANUALLY HERE
				float min = 0.0f;
				float max = 0.0f;
				channel->get3DMinMaxDistance(&min, &max);
				//std::cout << "setting sound pos" << std::endl;

				//std::cout << "sound pos min: " << min << " sound pos max: " << max << std::endl;

				//debugAudioPosition(position, channel);

				/*FMOD_VECTOR listener;
				core_system->get3DListenerAttributes(0, &listener, nullptr, nullptr, nullptr);
				Vec3D listener_pos = { listener.x, listener.y, listener.z };
				float distance = sqrt(pow(listener_pos.x - pos.x, 2) + pow(listener_pos.y - pos.y, 2) + pow(listener_pos.z - pos.z, 2));
				LM.write_log("Listener position: (%f, %f, %f)", listener_pos.x, listener_pos.y, listener_pos.z);
				LM.write_log("Sound position: (%f, %f, %f)", pos.x, pos.y, pos.z);
				LM.write_log("Calculated distance: %f", distance);*/
			}
			
		}
	}

	//this is for debug purpose to see what channels are actually playing
	void Audio_System::get_active_channels() {
		for (const auto& [key, channels] : channel_map) {
			bool playing = false;

			for (FMOD::Channel* channel : channels) {
				channel->isPlaying(&playing);
				if (playing) {
					LM.write_log("Active channel %s", key.c_str());
				}				
			}

			//std::cout << key << " channel size: " << channels.size() << std::endl;
		}
	}

	//this is for debug purpose to see what channels are actually playing
	void Audio_System::get_muted_channels() {
		for (const auto& [key, channels] : channel_map) {
			bool muted = false;

			for (FMOD::Channel* channel : channels) {
				channel->getMute(&muted);
				if (muted) {
					LM.write_log("Current channek %s is muted", key.c_str());
				}

			}

			//std::cout << key << " channel size: " << channels.size() << std::endl;
		}
	}

	void Audio_System::debug_list_active_sounds() {
		for (const auto& [key, channels] : channel_map) {
			for (FMOD::Channel* channel : channels) {
				FMOD::Sound* sound = nullptr;
				channel->getCurrentSound(&sound);
				if (sound) {
					char sound_name[512];
					sound->getName(sound_name, sizeof(sound_name));
					LM.write_log("Active Sound: %s (key: %s)", sound_name, key.c_str());
				}
			}
		}
	}

	//need rethink this one
	bool Audio_System::is_sound_playing(std::string& channel_key) {
		auto it = channel_map.find(channel_key);

		//if it has been checked that it is not in the channelmap it means it have yet to be played or it has finished and been removed
		if (it == channel_map.end()) {
			return false;
		}

		bool isplaying = false;

		for (FMOD::Channel* channel : it->second) {
			channel->isPlaying(&isplaying);
			if (isplaying) {
				return true;
			}
		}

		return false;
	}

	std::string Audio_System::get_type() const {
		return "Audio_System";
	}

	void Audio_System::debugAudioPosition(const Vec3D& sound_pos, FMOD::Channel* channel) {
		FMOD_VECTOR listener;
		core_system->get3DListenerAttributes(0, &listener, nullptr, nullptr, nullptr);
		Vec3D listener_pos = { listener.x, listener.y, listener.z };
		float distance = distance_vec3d(listener_pos, sound_pos);
		float min_dist = 0.0f;
		float max_dist = 0.0f;
		channel->get3DMinMaxDistance(&min_dist, &max_dist);

		std::cout << "========== Audio Debug Info ==========\n";
		std::cout << "Listener Position: (" << listener_pos.x << ", " << listener_pos.y << ", " << listener_pos.z << ")\n";
		std::cout << "Sound Position: (" << sound_pos.x << ", " << sound_pos.y << ", " << sound_pos.z << ")\n";
		std::cout << "Distance: " << distance << "\n";
		std::cout << "Min Distance: " << min_dist << " | Max Distance: " << max_dist << "\n";

		if (distance < min_dist) {
			std::cout << "Status: Listener is inside the min distance (full volume).\n";
		}
		else if (distance > max_dist) {
			std::cout << "Status: Listener is outside the max distance (sound should be silent).\n";
		}
		else {
			std::cout << "Status: Listener is within range (volume attenuating based on distance).\n";
		}
		std::cout << "======================================\n";
	}

	
}