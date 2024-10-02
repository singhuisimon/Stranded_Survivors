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

}


#endif
