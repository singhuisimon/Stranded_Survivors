//#ifndef _AUDIO_MANAGER_H
//#define _AUDIO_MANAGER_H

#include "../Entity/Entity.h"

#include "Manager.h"
#include "fmod.hpp"
#include "fmod_studio.hpp"
#include <iostream>
#include "vector"

namespace lof {

	//to be attached to entities that need sound
	struct Audio_Component {
		FMOD::Studio::EventInstance* event_instance = nullptr;	//FMOD event instance to play sound events
		std::string event_path;									//Path to Fmod event or sound file
		float volume = 1.0f;
		bool is_looping = false;
	};
	
	//manages fmod system and loading sound event
	//handling global settings like master volume
	class Audio_Manager : public Manager {
	public:
		//singleton pattern for gloabl audio_manager access
		static Audio_Manager& get_instance();

		//initialize fmod studio system
		void init();

		//play background music using an FMOD event path
		void play_bgmmusic(const std::string& bgm_eventpath);

		//stop playing the current bgm music
		void stop_bgmmusic();

		//grants direct access to FMOD system without requiring all operation done through audio_manager
		FMOD::Studio::System* get_studio_system();

		//update fmod to keep it synchronize
		void update();

		//shutdown fmod and clean up resource
		void shut_down();

	private:
		FMOD::Studio::System* studio_system;
		FMOD::Studio::EventInstance* bgm_music_instance;
		Audio_Manager(); //Private Constructor for singleton (only one instance of it throughout game)
	};


}


//#endif
