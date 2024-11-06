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
		Audio_System();
		~Audio_System();

		int errorcheck(FMOD_RESULT result, const std::string& function_name = "", const std::string& function_purpose = "");

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
		void play_sound(const std::string& file_path, std::string& cskey, std::string& audio_key,Audio_Component& entity);

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

		void stop_mastergroup();

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
		//FMOD::Studio::System* studio_system;	//Studio audio system

		void initializegroups();

		FMOD::ChannelGroup* bgmgroup;
		FMOD::ChannelGroup* sfxgroup;
		FMOD::ChannelGroup* mastergroup;

		std::unordered_map<std::string, FMOD::Sound*> sound_map;
		std::unordered_map<std::string, FMOD::Channel*> channel_map;
	};
}

#endif

