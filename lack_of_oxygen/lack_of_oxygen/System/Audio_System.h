/**
 * @file Audio_System.h
 * @brief Declaration of the Audio_System class for managing audio playback using FMOD.
 * @author Amanda Leow Boon Suan (100%)
 * @date September 27, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef _AUDIO_SYSTEM_H_
#define _AUDIO_SYSTEM_H_

 // Include FMOD headers
#include <fmod.hpp>
//#include <fmod_studio.hpp>
#include "fmod_errors.h"

// Include standard headers
#include <string>
#include <iostream>

//Include other necessary header
#include "../Manager/Log_Manager.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../System/System.h"

namespace lof {

	class Audio_System : public System
	{
	public:
		/**
		* @brief Default constructor
		*/
		Audio_System();

		/**
		* @brief Destructor
		*/
		~Audio_System();

		/**
		* @brief Prints out error or successful message depending on result.
		* @param result FMOD will detect if action is successful.
		* @function_name Function name where errrorcheck is being called from
		* @function_purpose The purpose of the function / The purpose of checking if there is any error
		*/
		int errorcheck(FMOD_RESULT result, const std::string& function_name = "", const std::string& function_purpose = "");

		/**
		* @brief Initializer for the Audio System
		*/
		bool initialize();

		/**
		* @brief Update for Audio System
		* @param delta_time
		*/
		void update(float delta_time) override;

		/**
		* @brief Shutdown for Audio System, releases all the resources etc.
		*/
		void shutdown();

		/**
		 * @brief Loads a sound from the specified file_path into the FMOD system
		 * @param file_path The path to the audio file (current format .wav)
		 */
		void load_sound(const std::string& file_path);

		/**
		 * @brief Plays the provided FMOD::Sound object.
		 * @param filepath The filepath to the audio
		 * @param cskey The unique id of filepath + entity id to be used as the channel key
		 * @param audio_key The unique identifier of the soundconfig
		 * @param entity A reference to a Audio Component.
		 */
		void play_sound(const std::string& file_path, std::string& cskey, std::string& audio_key, Audio_Component& entity);

		/**
		* @brief Pause and Resume sound
		* @param channel_key The unique identifier of the channel in the map
		* @pause True if the action is to pause the sound, false otherwise
		*/
		void pause_resume_sound(const std::string& channel_key, bool pause);

		/**
		* @brief Stop the sound
		* @param channel_key The unique identifier of the channel
		*/
		void stop_sound(const std::string& channel_key);

		/**
		* @brief Unload the sound
		* @param filepath The filepath and the key to find the sound in soundmap
		*/
		void unload_sound(const std::string& filepath);

		/**
		* @brief Setter for the channel pitch
		* @param channel_key The unique identifier of the channel in the map
		* @param pitch The new pitch value
		*/
		void set_channel_pitch(const std::string& channel_key, float pitch);

		/**
		* @brief Setter for the channel volume
		* @param channel_key The unique identifier of the channel in the map
		* @param volume The new volume value
		*/
		void set_channel_volume(const std::string& channel_key, float volume);
		/**
		* @brief Pause the channels under the bgm channelgroup
		*/
		void pause_bgm_group();

		/**
		* @brief Resume the channels under the bgm channelgroup
		*/
		void resume_bgm_group();

		/**
		* @brief Pause the channels under the sfx channelgroup
		*/
		void pause_sfx_group();

		/**
		* @brief Resume the channels under the sfx channelgroup
		*/
		void resume_sfx_group();

		/**
		* @brief Stop all the channel in the master channelgroup
		*/
		void stop_mastergroup();

		/**
		* @brief Getter for the bgm channelgroup volume
		*/
		float get_bgmgroup_volume() const;

		/**
		* @brief Setter for the bgm channelgroup volume
		* @param volume The new volume value
		*/
		void set_bgmgroup_volume(float volume);

		/**
		* @brief Getter for the bgm channelgroup pitch
		*/
		float get_bgmgroup_pitch() const;

		/**
		* @brief Setter for the bgm channelgroup pitch
		* @param pitch The new pitch value
		*/
		void set_bgmgroup_pitch(float pitch);

		/**
		* @brief Getter for the sfx channelgroup volume
		*/
		float get_sfxgroup_volume() const;

		/**
		* @brief Setter for the sfx channelgroup volume
		* @param volume The new volume value
		*/
		void set_sfxgroup_volume(float volume);

		/**
		* @brief Getter for the sfx channelgroup pitch
		*/
		float get_sfxgroup_pitch() const;

		/**
		* @brief Setter for the sfx channelgroup pitch
		* @param pitch The new pitch value
		*/
		void set_sfxgroup_pitch(float pitch);

		/**
		* @brief Getter for the mastergroup volume
		*/
		float get_mastergroup_volume() const;

		/**
		* @brief Setter for the mastergroup volume
		* @param volume The new volume value
		*/
		void set_mastergroup_volume(float volume);

		/**
		* @brief Getter for the mastergroup volume
		*/
		float get_mastergroup_pitch() const;

		/**
		* @brief Setter for the mastergroup pitch
		* @param pitch The new pitch value
		*/
		void set_mastergroup_pitch(float pitch);

		/**
		* @brief Getters for the bgmgroup
		*/
		FMOD::ChannelGroup* get_bgmgroup();

		/**
		* @brief Getters for the sfxgroup
		*/
		FMOD::ChannelGroup* get_sfxgroup();

		/**
		* @brief Getters for the mastergroup
		*/
		FMOD::ChannelGroup* get_mastergroup();

		//3D related
		//FMOD_VECTOR 
		//void set_listener_position(const Vec3D& pos);
		//void set_channel3D_pos(const std::string& channel_key, Vec3D& pos);

		/**
		* @brief Returns the type of the audio system
		* @return string representing the type
		*/
		std::string get_type() const override;

	private:

		FMOD::System* core_system;	///<Core audio system

		/**
		* @brief Initializes all the channel group as well as to add them under the master channel group
		*/
		void initializegroups();

		FMOD::ChannelGroup* bgmgroup;		///< Handles all the channel classified as background music
		FMOD::ChannelGroup* sfxgroup;		///< Handles all the channel classified as sound effect
		FMOD::ChannelGroup* mastergroup;	///< Handles all the channel that is running in the system.

		std::unordered_map<std::string, FMOD::Sound*> sound_map;		///< Map of Sound with the filepath as the key
		std::unordered_map<std::string, FMOD::Channel*> channel_map;	///< Map of Channel with the filepath + entityid as the key
	};
}

#endif

