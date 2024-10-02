/**
 * @file Audio_Manager.h
 * @brief 
 * @author Amanda Leow Boon Suan
 * @date September 27, 2024
 */

#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include "../Entity/Entity.h"
#include "../Manager/Log_Manager.h"
#include "../System/Audio_System.h"

#include "Manager.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include "vector"

namespace lof {

#define AM  lof::Audio_Manager::get_instance()

	class Audio_System;

	class Audio_Manager : public Manager{
	public:

		static Audio_Manager& get_instance();

		Audio_Manager();
		~Audio_Manager();

		int start_up() override;
		void update();
		void shutdown();

		void load_bgm(const std::string& track1_path, const std::string& track2_path);
		void play_bgm(int track_number);
		void stop_bgm();

	private:
		Audio_System* audio_system;
		FMOD::Sound* track1;
		FMOD::Sound* track2;
		FMOD::Channel* current_channel;
	};

	//class Audio_System;

	////to be attached to entities that need sound
	//struct Audio_Component {
	//	FMOD::Studio::EventInstance* event_instance = nullptr;	//FMOD event instance to play sound events
	//	std::string event_path;									//Path to Fmod event or sound file
	//	float volume = 1.0f;
	//	bool is_looping = false;
	//};

	//class Audio_Manager : public Manager {
	//public:
	//	static Audio_Manager& get_instance();

	//	//initialize the audio system
	//	int start_up() override;

	//	//load an audio bank through the audio_system (Studio)
	//	bool load_audio_bank(const std::string& bank_path);

	//	//load and register an event from the system (studio)
	//	bool load_audio_event(const std::string& event_name, const std::string& event_path);
	//	
	//	//load and register a sound (core)
	//	bool load_core_sound(const std::string& sound_name, const std::string& sound_path, bool is_3d = false, bool is_looping = false, bool is_steam = false);
	//	
	//	//play a specific sound event by name (studio)
	//	void play_sound(const std::string& event_name);

	//	//play a specific core sound by name (core)
	//	void play_core_sound(const std::string& sound_name);

	//	//stop a specific sound event by name (studio)
	//	void stop_sound(const std::string& event_name);

	//	//stop a core sound by name (core)
	//	void stop_core_sound(const std::string& sound_name);

	//	//play background music event (studio)
	//	void play_bgmusic(const std::string& bgm_eventname);

	//	//stop background music
	//	void stop_bgmusic(const std::string& bgm_event_name);

	//	//attach an audio component to an entity
	//	void attach_audio_component(Entity& entity, const std::string& event_path, bool is_loop, float volume = 1.0f);
	//	
	//	//play the sound attached to the entity's audio component
	//	void play_entity_sound(Entity& entity);

	//	// stop the sound attached to the entity's audio component
	//	void stop_entity_sound(Entity& entity);
	//	
	//	//! update the audio system
	//	void update();

	//	//! shut down audio system
	//	void shut_down();

	//private:
	//	Audio_Manager();
	//	~Audio_Manager();

	//	Audio_System* audio_system;

	//	std::unordered_map<std::string, FMOD::Studio::Bank*> bank_map;

	//	std::unordered_map<std::string, FMOD::Studio::EventInstance*> event_map;

	//	std::unordered_map<std::string, FMOD::Sound*> core_sound_map;

	//	std::unordered_map<EntityID, Audio_Component> entity_audio_map;

	//};


	////manages fmod system and loading sound event
	////handling global settings like master volume
	//class Audio_Manager : public Manager {
	//public:
	//	//singleton pattern for gloabl audio_manager access
	//	static Audio_Manager& get_instance();

	//	//initialize fmod studio system
	//	int start_up() override;

	//	//load path into bank
	//	void create_sound(const std::string& audio_path);

	//	//play background music using an FMOD event path
	//	void play_bgmmusic(const std::string& bgm_eventpath);

	//	//stop playing the current bgm music
	//	void stop_bgmmusic();

	//	//grants direct access to FMOD system without requiring all operation done through audio_manager
	//	FMOD::Studio::System* get_studio_system();

	//	//update fmod to keep it synchronize
	//	void update();

	//	//shutdown fmod and clean up resource
	//	void shut_down() override;

	//private:
	//	FMOD::Sound* psound;
	//	FMOD::Studio::System* studio_system = nullptr;
	//	FMOD::Studio::EventInstance* bgm_music_instance;
	//	Audio_Manager(); //Private Constructor for singleton (only one instance of it throughout game)
	//};


}


#endif
