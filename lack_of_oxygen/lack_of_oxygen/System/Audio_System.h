//#pragma once

#ifndef _AUDIO_SYSTEM_H_
#define _AUDIO_SYSTEM_H_

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include "fmod_errors.h"
#include "fmod_dsp.h"
#include <string>
#include <iostream>
//#include "../Entity/Entity.h"
#include "../Manager/Audio_Manager.h"



namespace lof {
	//! responsible for playing, stopping and updating sounds associated
	class Audio_System {
	public:

		Audio_System();
		~Audio_System();

		bool initialize();
		void update();
		void shutdown();

		FMOD::Sound* load_sound(const std::string& file_path);
		FMOD::Channel* play_sound(FMOD::Sound* sound, bool loop = false);
		void stop_all_sounds();

	private:

		FMOD::System* core_system;
		//FMOD_RESULT result;
	};

}

#endif

//class Audio_System {
//public:
//
//	Audio_System();		//constructor
//	~Audio_System();	//destructor
//
//	//static Audio_System& get_instance();
//	bool initialize();
//
//	//load audio bank file (for FMOD Studio)
//	bool load_bank(const std::string& bank_path, FMOD::Studio::Bank** bank);
//
//	//load a sound using FMOD Core
//	FMOD::Sound* load_sound(const std::string& sound_path, bool is_3d = false, bool is_looping = false, bool is_stream = false);
//
//	//Play a sound (Core System)
//	void play_core_sound(FMOD::Sound* sound, FMOD::Channel** channel);
//
//	//Load an event description by path (For FMOD Studio)
//	FMOD::Studio::EventDescription* get_event_description(const std::string& event_path);
//
//	//Create an event instance from an event description (FMOD Studio)
//	FMOD::Studio::EventInstance* create_event_instance(FMOD::Studio::EventDescription* event_desc);
//
//	//Update the FMOD System
//	void update();
//
//	//Shutdown the FMOD system and clean resources
//	void shut_down();
//
//	FMOD::Studio::System* get_studio_system() { return studio_system; }
//	FMOD::System* get_core_system() { return core_system; }
//
//private:
//	FMOD::Studio::System* studio_system;	//! FMOD Studio System
//	FMOD::System* core_system;				//! FMOD Core System
//	bool is_shut_down = false;
//};

//namespace lof {
//
//	//responsible for playing, stopping, and updating sounds associated with entities
//	class Audio_System {
//
//	public:
//		//initialize audio system
//		void init();
//
//		//update all entities with audio component
//		void update();
//
//		//play sound for a specific entity
//		void play_sound(Audio_Component& audio_comp);
//
//		//stop sound for a specific entity
//		void stop_sound(Audio_Component& audio_comp);
//
//		void add_entity(EntityID entity);
//
//		//shutdown and clean up
//		void shut_down();
//
//		void register_entity(Entity entity);
//
//	private:
//		FMOD::Studio::System* studio_system;
//		std::vector<Entity> entities_with_audio;	//list of entities that have audio
//
//		//mock function ->replace with real logic
//		Audio_Component* get_audio_component(Entity entity) {
//			return nullptr;
//		}
//	};
//}
