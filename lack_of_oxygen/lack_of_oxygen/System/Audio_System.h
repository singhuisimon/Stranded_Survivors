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

