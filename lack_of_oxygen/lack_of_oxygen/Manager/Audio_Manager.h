/**
 * @file Audio_Manager.h
 * @brief Defines the Audio Manager class.
 * @author Amanda Leow Boon Suan (98%), Liliana Hanawardani (2%)
 * @date January 15, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once

#ifndef LOF_AUDIO_MANAGER_H
#define LOF_AUDIO_MANAGER_H

#define ADM lof::Audio_Manager::get_instance()

#include <fmod.hpp>
#include <fmod_errors.h>

#include "../Component/Component.h"

#include "../Utility/Constant.h"
#include "../Utility/Globals.h"

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

	class Audio_Manager : public Manager {
	public:

		/**
		 * @brief Get the unique instance of Audio_Manager.
		 */
		static Audio_Manager& get_instance();
		
		/**
		 * @brief Copy constructor and copy assignment operator
		 */
		Audio_Manager(const Audio_Manager&) = delete;
		Audio_Manager& operator=(const Audio_Manager&) = delete;

		/**
		* @brief Initializes all the channel group as well as to add them under the master channel group
		*/
		void initializegroups();

		/**
		 * @brief Initialize the audio manager
		 */
		int start_up() override;

		/**
		* @brief Prints out error or successful message depending on result.
		* @param result FMOD will detect if action is successful.
		* @function_name Function name where errrorcheck is being called from
		* @function_purpose The purpose of the function / The purpose of checking if there is any error
		*/
		static int errorcheck(FMOD_RESULT result, const std::string& function_name = "", const std::string& function_purpose = "");

		/**
		 * @brief Initialize the FMOD system
		 * @return bool if initialization of fmod system is a success
		 */
		bool initialize();

		/**
		 * @brief Release all the resources
		 */
		void shut_down();

		//sound loading
		/**
		 * @brief Loads a sound from the specified file_path into the FMOD system
		 * @param file_path The path to the audio file (current format .wav)
		 * @param audio_type The type of audio (BGM, SFX, UI)
		 * @param is3d True if the audio is 3D, false otherwise
		 */
		void load_sound(const std::string& file_path, const AudioType audio_type, bool is3d = false);

		/**
		* @brief Unload the sound
		* @param filepath The filepath and the key to find the sound in soundmap
		*/
		void unload_sound(const std::string& file_path);

		/**
		 * @brief Retrieve the sound from the sound map.
		 * @param file_path The path to the audio file.
		 * @param audio_type The type of audio (BGM, SFX, UI)
		 * @param is3d True if the audio is 3D, false otherwise
		 * @return FMOD::Sound* The sound object.
		 */
		FMOD::Sound* get_sound(const std::string& file_path, AudioType audio_type, bool is3d);

		/**
		 * @brief Play the sound immediately.
		 * @param entity_id The entity id of the entity playing the sound.
		 * @param audio_key The key of the audio to be played.
		 * @param audio_component The audio component of the entity.
		 * @param bgm True if the sound is a background music, false otherwise.
		 */
		void play_now(EntityID entity_id, const std::string& audio_key, Audio_Component& audio_component, bool bgm = false);

		/**
		 * @brief Stop the sound immediately.
		 * @param entity_id The entity id of the entity playing the sound.
		 * @param audio_key The key of the audio to be stopped.
		 * @param file_path The file path of the audio to be stopped.
		 */
		void stop_now(EntityID entity_id, const std::string& audio_key, const std::string& file_path);

		/**
		 * @brief Mute the sound layer.
		 * @param entity_id The entity id of the entity playing the sound.
		 * @param audio_key The key of the audio to be muted.
		 * @param file_path The file path of the audio to be muted.
		 */
		//void mute_layer(EntityID entity_id, const std::string& audio_key, const std::string& file_path);

		/**
		 * @brief Unmute the sound layer.
		 * @param entity_id The entity id of the entity playing the sound.
		 * @param audio_key The key of the audio to be 
		 
		 
		 
		 
		 .
		 * @param file_path The file path of the audio to be unmuted.
		 */
		//void unmute_layer(EntityID entity_id, const std::string& audio_key, const std::string& file_path);

		/**
		 * @brief Update the layering of the background music based on the oxygen level.
		 * @param current_scene The current scene number.
		 * @param oxygen_level The current oxygen level.
		 * @param increasing True if the oxygen level is increasing, false otherwise.
		 */
		void update_bgm_layering(const int current_scene, const float oxygen_level = 100, bool increasing = false);

		/**
		 * @brief Check if the layer is playing.
		 * @param entity_id The entity
		 */
		bool is_layer_playing(EntityID entity_id, const std::string& audio_key);

		//getters for channelgroup

		/**
		 * @brief Get the master channel group
		 */
		FMOD::ChannelGroup* get_mastergroup() const;

		/**
		 * @brief Get the bgm channel group
		 */
		FMOD::ChannelGroup* get_bgmgroup() const;

		/**
		 * @brief Get the sfx channel group
		 */
		FMOD::ChannelGroup* get_sfxgroup() const;

		/**
		 * @brief Get the ui channel group
		 */
		FMOD::ChannelGroup* get_uigroup() const;

		//stop all audio -> to be called before each scene change!
		/**
		* @brief Stop all the channel in the master channelgroup
		*/
		void stop_mastergroup();

		void stop_groups(GroupType grouptype);

		/**
		 * @brief Pause the channel in the master channelgroup
		 */
		void pause_resume_mastergroup();

		//channel group control

		/**
		 * @brief Pause the group
		 * @param grouptype The type of group to pause
		 */
		void pause_group(GroupType grouptype);

		/**
		 * @brief Resume the group
		 * @param grouptype The type of group to resume
		 */
		void resume_group(GroupType grouptype);

		/**
		 * @brief Set the group volume
		 * @param grouptype The type of group to set the volume
		 * @param volume The volume to set
		 */
		void set_group_volume(GroupType grouptype, float volume);

		/**
		 * @brief Get the group volume
		 * @param grouptype The type of group to get the volume
		 */
		float get_group_volume(GroupType grouptype);

		/**
		 * @brief Get the sound map filename
		 * @return a vector of string containing the filename
		 */
		std::vector<std::string> get_sound_map_filename() const;

		/**
		 * @brief Get the sound map
		 * @return a unordered map of string and FMOD::Sound* containing the sound map
		 */
		std::unordered_map<std::string, FMOD::Sound*>& get_sound_map() { return sound_map; }

		/**
		 * @brief Get the core system
		 * @return the core system
		 */
		std::string mode_to_string(FMOD_MODE mode);

		/**
		 * @brief Set the new_scene flag
		 * @param new_scene_state The new state of the new_scene flag
		 */
		void set_new_scene(bool new_scene_state);

		/**
		 * @brief Get the new_scene flag
		 * @return a boolean value consisting the value of the new_scene flag
		 */
		bool get_new_scene();

		void debug_group_sound() {
			std::cout << "Mastergroup " << get_group_volume(GroupType::TYPE_MASTER) << std::endl;
			std::cout << "BGM group " << get_group_volume(GroupType::TYPE_BGM) << std::endl;
			std::cout << "SFX group " << get_group_volume(GroupType::TYPE_SFX) << std::endl;
		}

	private:

		/**
		 * @brief Constructor
		 */
		Audio_Manager();

		/**
		 * @brief Destructor
		 */
		~Audio_Manager();

		/**
		 * @brief The core system for FMOD
		 */
		FMOD::System* core_system;

		std::unordered_map<std::string, FMOD::Sound*> sound_map;	//filename is key

		//channel groups
		FMOD::ChannelGroup* mastergroup;
		FMOD::ChannelGroup* bgmgroup;
		FMOD::ChannelGroup* sfxgroup;
		FMOD::ChannelGroup* uigroup;

		//flag for new scene
		bool new_scene;
	};
}

#endif