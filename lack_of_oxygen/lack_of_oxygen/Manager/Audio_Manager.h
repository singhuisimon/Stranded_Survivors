#pragma once
#pragma once

#ifndef LOF_AUDIO_MANAGER_H
#define LOF_AUDIO_MANAGER_H

#define ADM lof::Audio_Manager::get_instance()

#include <fmod.hpp>
#include <fmod_errors.h>

#include "../Component/Component.h"

#include "../Utility/Constant.h"
#include "../Utility/globals.h"

#include "../System/Audio_System.h"

#include "../Manager/Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Game_Manager.h"
#include "../Manager/Assets_Manager.h"

#include <unordered_map>
#include <iostream>
#include <string>
#include <queue>

namespace lof {

	struct AudioEvent;

	class Audio_Manager : public Manager {
	public:
		static Audio_Manager& get_instance();

		Audio_Manager(const Audio_Manager&) = delete;
		Audio_Manager& operator=(const Audio_Manager&) = delete;

		/**
		* @brief Initializes all the channel group as well as to add them under the master channel group
		*/
		void initializegroups();


		int start_up() override;

		/**
		* @brief Prints out error or successful message depending on result.
		* @param result FMOD will detect if action is successful.
		* @function_name Function name where errrorcheck is being called from
		* @function_purpose The purpose of the function / The purpose of checking if there is any error
		*/
		static int errorcheck(FMOD_RESULT result, const std::string& function_name = "", const std::string& function_purpose = "");

		bool initialize(); //Initialize FMOD System
		void shut_down(); //Shut down FMOD systems and release resources

		//sound loading
		/**
		 * @brief Loads a sound from the specified file_path into the FMOD system
		 * @param file_path The path to the audio file (current format .wav)
		 */
		void load_sound(const std::string& file_path, const AudioType audio_type, bool is3d = false);

		/**
		* @brief Unload the sound
		* @param filepath The filepath and the key to find the sound in soundmap
		*/
		void unload_sound(const std::string& file_path);

		FMOD::Sound* get_sound(const std::string& file_path, AudioType audio_type, bool is3d);

		void play_now(EntityID entity_id, const std::string& audio_key, Audio_Component& audio_component, bool bgm = false);

		void stop_now(EntityID entity_id, const std::string& audio_key, const std::string& file_path);

		void mute_layer(EntityID entity_id, const std::string& audio_key, const std::string& file_path);

		void unmute_layer(EntityID entity_id, const std::string& audio_key, const std::string& file_path);

		void update_bgm_layering(const int current_scene, const float oxygen_level = 100, bool increasing = false);

		bool is_layer_playing(EntityID entity_id, const std::string& audio_key);

		//getters for channelgroup
		FMOD::ChannelGroup* get_mastergroup() const;
		FMOD::ChannelGroup* get_bgmgroup() const;
		FMOD::ChannelGroup* get_sfxgroup() const;
		FMOD::ChannelGroup* get_uigroup() const;

		//stop all audio -> to be called before each scene change!
		/**
		* @brief Stop all the channel in the master channelgroup
		*/
		void stop_mastergroup();

		void pause_resume_mastergroup();

		//channel group control
		void pause_group(GroupType grouptype);
		void resume_group(GroupType grouptype);
		void set_group_volume(GroupType grouptype, float volume);
		float get_group_volume(GroupType grouptype);

		std::vector<std::string> get_sound_map_filename() const;
		std::string modeToString(FMOD_MODE mode);


	private:

		Audio_Manager();
		~Audio_Manager();

		FMOD::System* core_system;

		std::unordered_map<std::string, FMOD::Sound*> sound_map;	//filename is key

		FMOD::ChannelGroup* mastergroup;
		FMOD::ChannelGroup* bgmgroup;
		FMOD::ChannelGroup* sfxgroup;
		FMOD::ChannelGroup* uigroup;

		bool new_scene;

	};
}

#endif