/**
 * @file Audio_Manager.cpp
 * @brief Define of the Audio_Manager class functions.
 * @author Amanda Leow Boon Suan (100%)
 * @date September 27, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "../Manager/Audio_Manager.h"
#include "../Utility/Path_Helper.h"

namespace lof {

	//singleton
	Audio_Manager& Audio_Manager::get_instance() {
		static Audio_Manager instance;
		return instance;
	}

	//constructor
	Audio_Manager::Audio_Manager() : audio_system(nullptr), track1(nullptr), track2(nullptr), current_channel(nullptr) {
		audio_system = new Audio_System();
		set_type("Audio_Manager");
	}

	//destructor
	Audio_Manager::~Audio_Manager() {
		shutdown();
		delete audio_system;
		audio_system = nullptr;
	}

	//initialize the system and start up the manager
	int Audio_Manager::start_up() {
		if (is_started()) {
			return 0; // Already started
		}

		//initialize the audio_system
		if (!audio_system->initialize()) {
			LM.write_log("Audio_Manager::start_up(): Failed to initialize Audio_Manager");
			return -1;
		}

		const std::string music_path1 = Path_Helper::get_music_path1();
		const std::string music_path2 = Path_Helper::get_music_path2();

		//load the background music
		this->load_bgm(music_path1, music_path2);

		m_is_started = true;
		return 0;
	}

	//update the audio_system
	void Audio_Manager::update() {
		audio_system->update();
	}

	//shutdown the audio system and audio manager
	void Audio_Manager::shutdown() {
		if (!is_started()) {
			return; // Not started
		}

		if (track1) {
			track1->release();
			track1 = nullptr;
		}
		if (track2) {
			track2->release();
			track2 = nullptr;
		}
		audio_system->shutdown();

		LM.write_log("Audio_Manager::shutdown() Successfully shutdown audio");

		m_is_started = false;
	}

	//load the background music track onto the system
	void Audio_Manager::load_bgm(const std::string& track1_path, const std::string& track2_path) {
		track1 = audio_system->load_sound(track1_path);

		//check if track1 failed to load
		if (!track1) {
			LM.write_log("Audio_Manager::load_bgm() Fail to load track1.");
			return;
		}
		else {
			LM.write_log("Audio_Manager::load_bgm() Successful in loading the BGM track1.");
		}

		track2 = audio_system->load_sound(track2_path);

		//check if track2 failed to load
		if (!track2) {
			LM.write_log("Audio_Manager::load_bgm() Fail to load track2.");
			return;
		}
		else {
			LM.write_log("Audio_Manager::load_bgm() Successful in loading the BGM track2.");
		}
		
	}

	//play the background music based on the tracknumber indicated
	void Audio_Manager::play_bgm(int track_number) {
		//check if track_number is valid
		if (track_number < 1 || track_number > 2) {
			LM.write_log("Audio_Manager::play_bgm(): Invalid track number");
			return;
		}

		//if current channel has a sound playing stop the sound.
		if (current_channel) {
			current_channel->stop();
		}

		//check the track_number and if track exist. if so play that track
		if (track_number == 1 && track1) {
			current_channel = audio_system->play_sound(track1, true);
			LM.write_log("Audio_Manager::play_bgm(): Playing bgm track1.");
		}
		else if (track_number == 2 && track2) {
			current_channel = audio_system->play_sound(track2, true);
			LM.write_log("Audio_Manager::play_bgm(): Playing bgm track2");
		}
		else {
			//std::cerr << "Audio_Manager::play_bgm() invalid number of track/ track not loaded" << std::endl;
			LM.write_log("Audio_Manager::play_bgm() invalid number of track/ track not loaded");
		}
	}

	//stop the current playing bgm audio.
	void Audio_Manager::stop_bgm() {
		if (current_channel) {
			current_channel->stop();
			current_channel = nullptr;
			LM.write_log("Audio_Manager::stop_bgm(): Successfully stop bgm.");
		}
	}

	
}

