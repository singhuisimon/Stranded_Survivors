///**
// * @file Audio_Manager.h
// * @brief Declare the Audio_Manager class that functions as an interface (class wrapper) for Audio_System
// * @author Amanda Leow Boon Suan (100%)
// * @date September 27, 2024
// * Copyright (C) 2024 DigiPen Institute of Technology.
// * Reproduction or disclosure of this file or its contents without the
// * prior written consent of DigiPen Institute of Technology is prohibited.
// */
//
//#ifndef _AUDIO_MANAGER_H
//#define _AUDIO_MANAGER_H
//
//// Macros for accessing manager singleton instances
//#define AM  lof::Audio_Manager::get_instance()
//
//// Include base Manager class
//#include "Manager.h"
//
//// Include necessary headers from this project
//#include "../System/Audio_System.h"	//contains class Audio_System and all necessary FMOD headers
//#include "../Utility/Constant.h"
//#include "../Manager/Input_Manager.h"
//
//// Include standard headers
//#include <iostream>
//#include <string>
//
//
//
//namespace lof {
//
//	//forward declaration of the Audio_System class
//	//class Audio_System;
//
//	class Audio_Manager : public Manager {
//	public:
//		static Audio_Manager& get_instance();
//		~Audio_Manager();
//
//		int errorcheck(std::string function_name = "", std::string function_purpose = "", FMOD_RESULT result);
//
//		int start_up() override;
//		void update();
//		void shut_down() override;
//
//		void load_bank(const std::string& bank_name);
//		void load_event(const std::string& event_name);
//		void load_sound(const std::string& sound_name, bool is_3d = true, bool is_looping = false);
//		
//		void play_event(const std::string& event_name);
//		void stop_event(const std::string& event_name);
//		void pause_event(const std::string& event_name);
//		void resume_event(const std::string& event_name);
//		
//		int play_sound(const std::string& sound_name, float volume, bool is_looping);
//		void stop_sound(int channel_id);
//		void pause_sound(int channel_id);
//		void resume_sound(int channel_id);
//
//		void set_channel3D_pos(FMOD::Channel*& channel, Vec3D& pos);
//		void set_channel_volume(int channel_id, float volume);
//		void set_listener_position(const Vec3D& pos);
//
//		void pause_bgm_group();
//		void resume_bgm_group();
//
//		void pause_sfx_group();
//		void resume_sfx_group();
//
//		float get_bgmgroup_volume() const;
//		void set_bgmgroup_volume(float volume);
//		float get_bgmgroup_pitch() const;
//		void set_bgmgroup_volume(float volume);
//
//		float get_sfxgroup_volume() const;
//		void set_sfxgroup_volume(float volume);
//		float get_sfxgroup_pitch() const;
//		void set_sfxgroup_pitch(float volume);
//
//		float get_mastergroup_volume() const;
//		void set_mastergroup_volume(float volume);
//		float get_mastergroup_pitch() const;
//		void set_mastergroup_pitch(float volume);
//
//		FMOD::ChannelGroup* get_bgmgroup();
//		FMOD::ChannelGroup* get_sfxgroup();
//		FMOD::ChannelGroup* get_mastergroup();
//
//		FMOD::System* get_core_system() const;
//		FMOD::Studio::System* get_studio_system() const;
//
//	private:
//		Audio_Manager();
//
//		FMOD::System* core_system;	//Core audio system
//		FMOD::Studio::System* studio_system;	//Studio audio system
//
//		void initializegroups();
//
//		FMOD::ChannelGroup* bgmgroup;
//		FMOD::ChannelGroup* sfxgroup;
//		FMOD::ChannelGroup* mastergroup;
//		
//		int nextchannel_id;
//
//		//stores all the loaded sound and bank
//		std::unordered_map<std::string, FMOD::Sound*> sound_map;
//		std::unordered_map<int, FMOD::Channel*> all_channel_id;
//		std::unordered_map<std::string, FMOD::Studio::EventInstance*> eventinstance_map;
//		std::unordered_map<std::string, FMOD::Studio::Bank*> bank_map;
//	};
//
//		/*static Audio_Manager& get_instance();
//
//		Audio_Manager();
//		~Audio_Manager();
//
//		int start_up() override;
//		void update(float delta_time);
//		void shutdown();
//
//		void load_audio(const std::string& filepath, const std::string& sound_name);
//		void load_event(const std::string& event_name);
//
//		void play_sound(const std::string& sound_name);
//		void stop_sound(const std::string& sound_name);
//		void pause_sound(const std::string& sound_name);
//		void resume_sound(const std::string& sound_name);
//
//		void play_event(const std::string& event_name);
//		void stop_event(const std::string& event_name);
//		void pause_event(const std::string& event_name);
//		void resume_event(const std::string& event_name);
//
//	private:
//		std::unique_ptr<Audio_System> audio_system;
//		std::unordered_map<std::string, FMOD::Sound*> soundmap;
//		std::unordered_map<std::string, FMOD::Studio::EventInstance*> eventmap;
//		std::unordered_map<std::string, FMOD::Studio::Bank*> bankmap;
//		std::unordered_map<int, FMOD::Channel*> channelmap;*/
//	//};
//
//	///**
//	// * @class Audio_Manager
//	// * @brief Acts as an interface for Audio System as well as to load,play and stop background music.
//	// */
//	//class Audio_Manager : public Manager{
//	//public:
//
//	//	/**
//	//	 * @brief Get the singleton instance of the Audio_Manager.
//	//	 * @return Reference to the Audio_Manager instance.
//	//	 */
//	//	static Audio_Manager& get_instance();
//
//	//	/**
//	//	 * @brief Constructor for Audio_Manager
//	//	 */
//	//	Audio_Manager();
//
//	//	/**
//	//	 * @brief Destructor for Audio_Manager
//	//	 */
//	//	~Audio_Manager();
//
//	//	/**
//	//	 * @brief Starts up the Audio System
//	//	 * @return int Returns 0 on success, -1 on failure
//	//	 */
//	//	int start_up() override;
//
//	//	/**
//	//	 * @brief Updates the Audio System
//	//	 */
//	//	void update();
//
//	//	/**
//	//	 * @brief Shuts down the Audio System
//	//	 */
//	//	void shutdown();
//
//	//	/**
//	//	 * @brief Loads background music track into the Audio System
//	//	 * @param track1_path The file path to the first background music track.
//	//	 * @param track2_path The file path to the second background music track.
//	//	 */
//	//	void load_bgm(const std::string& track1_path, const std::string& track2_path);
//
//	//	/**
//	//	 * @brief Play the loaded background music track
//	//	 * @param track_number Indicator of which track to play (1 for track1, 2 for track2)
//	//	 */
//	//	void play_bgm(int track_number);
//
//	//	/**
//	//	 * @brief Stops the currently playing background music.
//	//	 */
//	//	void stop_bgm();
//
//	//private:
//	//	Audio_System* audio_system;		///< Pointer to the Audio_System that handles low-level audio operation
//	//	FMOD::Sound* track1;			///< Pointer to the first background music track
//	//	FMOD::Sound* track2;			///< Pointer to the second background music track
//	//	FMOD::Channel* current_channel;	///< Pointer to the current playing audio channel, used to control playback(looping)
//	//};
//
//}
//
//
//#endif
