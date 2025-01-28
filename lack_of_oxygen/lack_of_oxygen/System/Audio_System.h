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
#include "../Manager/Game_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/Assets_Manager.h"
#include "../Utility/Constant.h"
#include "../Utility/globals.h"
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

		//void debug_compressor_activity();
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

		static std::string generate_channel_key(EntityID entity_id, const std::string& file_path, const std::string& audio_key);

		/**
		 * @brief Plays the provided FMOD::Sound object.
		 * @param filepath The filepath to the audio
		 * @param cskey The unique id of filepath + entity id to be used as the channel key
		 * @param audio_key The unique identifier of the soundconfig
		 * @param entity A reference to a Audio Component.
		 */
		void play_sfx_sound(const std::string& file_path, std::string& cskey, const std::string& audio_key, const Audio_Component& audio);

		/**
		 * @brief Plays the provided FMOD::Sound object.
		 * @param filepath The filepath to the audio
		 * @param cskey The unique id of filepath + entity id to be used as the channel key
		 * @param audio_key The unique identifier of the soundconfig
		 * @param entity A reference to a Audio Component.
		 */
		void play_bgm_sound(const std::string& file_path, std::string& cskey, const std::string& audio_key, const Audio_Component& audio);

		//void update_bgm(); //for future when using it to do layering & detecting oxygen level

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

		//retrive the core_system
		FMOD::System* get_core_system();

		//TODO::APPLY CONST ON NON-EDITABLE DATA!!
		void apply_dist_effect(const std::string& channel_key, const Vec3D& listener_pos, const Vec3D& sound_pos);

		Vec3D get_channel_pos(const std::string& channel_key);
		Vec3D get_listener_pos();
		void update_audio_for_listener(Vec3D& listener_pos);
		void set_sound_position(const std::string& channel_key, const Vec3D position);

		void get_active_channels();
		bool is_sound_playing(std::string& channel_key);

		void fade_in(const std::string& channel_key, float duration);
		void fade_out(const std::string& channel_key, float duration);


		/**
		* @brief Returns the type of the audio system
		* @return string representing the type
		*/
		std::string get_type() const override;

	private:

		FMOD::System* core_system;	///<Core audio system

		std::unordered_map<std::string, std::vector<FMOD::Channel*>> channel_map;	///< Map of Channel with the filepath + entityid as the key
		std::unordered_map<std::string, std::string> all_prev_filepath_map;	///< Map of String with audio key as key and filepath as data

		//friend class Audio_Manager;
	};
}

#endif

