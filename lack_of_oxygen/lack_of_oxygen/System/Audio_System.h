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
#include <fmod_studio.hpp>
#include "fmod_errors.h"

// Include standard headers
#include <string>
#include <iostream>

//Include other necessary header
#include "../Manager/Log_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"
#include "../System/system.h"

namespace lof {

	class Audio_System : public System 
	{
	public:
		Audio_System();
		~Audio_System();

		int errorcheck(FMOD_RESULT result, std::string function_name = "", std::string function_purpose = "");

		bool initialize();
		void update(float delta_time) override;
		void shutdown();

		/**
		 * @brief Loads a sound from the specified file_path into the FMOD system
		 * @param file_path The path to the audio file (current format .wav)
		 */
		void load_sound(const std::string& file_path);

		/**
		 * @brief Plays the provided FMOD::Sound object.
		 * @param sound A pointer to the FMOD::Sound object.
		 * @param loop If true, sound will loop indefinitely, otherwise if false sound will only play once
		 *		  It is defaulted to be false.
		 */
		void play_sound(const std::string& file_path, std::string& entity_num);

		void pause_resume_sound(const std::string& channel_key, bool pause);
		void stop_sound(const std::string& channel_key);

		void unload_sound(const std::string& filepath);

		void set_channel_pitch(const std::string& channel_key, float pitch);
		void set_channel_volume(const std::string& channel_key, float volume);
		//did not add getters for pitch and volume for respective channel.

		void pause_bgm_group();
		void resume_bgm_group();

		void pause_sfx_group();
		void resume_sfx_group();

		float get_bgmgroup_volume() const;
		void set_bgmgroup_volume(float volume);
		float get_bgmgroup_pitch() const;
		void set_bgmgroup_pitch(float pitch);

		float get_sfxgroup_volume() const;
		void set_sfxgroup_volume(float volume);
		float get_sfxgroup_pitch() const;
		void set_sfxgroup_pitch(float pitch);

		float get_mastergroup_volume() const;
		void set_mastergroup_volume(float volume);
		float get_mastergroup_pitch() const;
		void set_mastergroup_pitch(float pitch);

		FMOD::ChannelGroup* get_bgmgroup();
		FMOD::ChannelGroup* get_sfxgroup();
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
		//Audio_Manager& audio_manager;
		FMOD::System* core_system;	//Core audio system
		FMOD::Studio::System* studio_system;	//Studio audio system

		void initializegroups();

		FMOD::ChannelGroup* bgmgroup;
		FMOD::ChannelGroup* sfxgroup;
		FMOD::ChannelGroup* mastergroup;

		std::unordered_map<std::string, FMOD::Sound*> sound_map;
		std::unordered_map<std::string, FMOD::Channel*> channel_map;
	};


//	/**
//	 * @class Audio_System
//	 * @brief Handles the initialization, playback and shutdown of audio using FMOD.
//	 *		  It is also responsible for loading sound, playing osund and updating the FMOD system.
//	 */
//	class Audio_System {
//	public:
//
//		/**
//		 * @brief Constructor for the Audio_System Class
//		 */
//		Audio_System();
//
//		/**
//		 * @brief Destructor for the Audio_System Class
//		 */
//		~Audio_System();
//
//		/**
//		 * @brief Initializes the FMOD core system for audio playback.
//		 * @return True if initialization is successful, otherwise false.
//		 */
//		bool initialize();
//
//		/**
//		 * @brief Updates the FMOD system
//		 */
//		void update();
//
//		/**
//		 * @brief Shuts down the FMOD system, releasing any loaded sound and resources.
//		 *	      Ensures that all sounds are stopped and the system is safely closed.
//		 */
//		void shutdown();
//
//		/**
//		 * @brief Loads a sound from the specified file_path into the FMOD system
//		 * @param file_path The path to the audio file (current format .wav)
//		 * @return A pointer to the loaded FMOD::Sound Object, or nullptr if loading fails.
//		 */
//		//for .wav
//		FMOD::Sound* load_sound(const std::string& file_path);
//
//		/**
//		 * @brief Plays the provided FMOD::Sound object.
//		 * @param sound A pointer to the FMOD::Sound object.
//		 * @param loop If true, sound will loop indefinitely, otherwise if false sound will only play once
//		 *		  It is defaulted to be false.
//		 * @return A pointer to the FMOD::Channel object controlling the playback of the sound.
//		 */
//		//for .wav
//		FMOD::Channel* play_sound(FMOD::Sound* sound, bool loop = false);
//
//		//for .bank
//		bool load_bank(const std::string& bank_path);
//		//the allow multiple instances refer to allowing the path file to play more than once. e.g. background
//		//u only want to play once set it to false. if its a gun sound effect set it to true as it will create a
//		//gun sound instance everytime the gunshot is triggered.
//		FMOD::Studio::EventInstance* play_event(const std::string& event_path, bool allow_multiple_instances);
//
//		//stop a specific sound by its channel
//		void stop_sound(FMOD::Channel* channel);
//
//		void pause_all_sound();
//		void resume_all_sound();
//
//		//stop all sounds
//		void stop_all_sound();
//
//		//set volume for a specific channel (BGM or SFX)
//		void set_volume(FMOD::Channel* channel, float volume);
//
//		//sound positioning
//		//void set_listener_position(Vec2D pos);
//		//void set_sound_position(FMOD::Channel* channel, Vec2D pos);
//
//	private:
//		FMOD::System* core_system; ///< Pointer to the FMOD core system responsible for managing audio playback
//		FMOD::Studio::System* studio_system;
//	};
//
}

#endif

