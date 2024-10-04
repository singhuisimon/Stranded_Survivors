/**
 * @file Audio_System.cpp
 * @brief Define of the Audio_System class for managing audio playback using FMOD.
 * @author Amanda Leow Boon Suan (100%)
 * @date September 27, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "Audio_System.h"

namespace lof {

	//Constructor for Audio_System
	Audio_System::Audio_System() : core_system(nullptr) {}

	//Destructor for Audio_System
	Audio_System::~Audio_System() {
		shutdown();
	}

	//initialize the FMOD core
	bool Audio_System::initialize() {

		FMOD_RESULT result;
		result = FMOD::System_Create(&core_system);

		//check if creation of the core system is successful
		if (result != FMOD_OK) {
			LM.write_log("Audio_System::initialize() Fail to create core system. Error: ");
			LM.write_log(FMOD_ErrorString(result));
			return false;
		}

		result = core_system->init(512, FMOD_INIT_NORMAL, 0);

		//check if the initialization of the core system is successful
		if (result != FMOD_OK) {
			LM.write_log("Audio_System::initialize(): Fail to initalize system. Error:");
			LM.write_log(FMOD_ErrorString(result));
			return false;
		}

		LM.write_log("Audio_System::initialize(): FMOD system successfully initialize.");

		return true;
	}

	//updates the FMOD system
	void Audio_System::update() {
		core_system->update();
	}

	//Loads the audio file into a FMOD::Sound object
	FMOD::Sound* Audio_System::load_sound(const std::string& file_path) {
		FMOD::Sound* sound = nullptr;
		FMOD_MODE mode = FMOD_DEFAULT;
		FMOD_RESULT result = core_system->createSound(file_path.c_str(), mode, 0, &sound);
		
		//check if the creation of sound using the path, object mode is successful
		if (result != FMOD_OK) {
			LM.write_log("Audio_System::load_sound(): Failed to create sound. Error: ");
			LM.write_log(FMOD_ErrorString(result));
			return nullptr;
		}

		LM.write_log("Audio_Sytem::load_sound(): Successfully loaded the sound from system.");
		return sound;
	}

	//play the FMOD::Sound Object by returning a channel that controls this playback
	FMOD::Channel* Audio_System::play_sound(FMOD::Sound* sound, bool loop) {
		FMOD::Channel* channel = nullptr;

		//check if loop is true
		if (loop) {
			sound->setMode(FMOD_LOOP_NORMAL);
		}
		else {
			sound->setMode(FMOD_LOOP_OFF);
		}
		
		FMOD_RESULT result = core_system->playSound(sound, nullptr, false, &channel);

		//check if sound object can be played through the channel
		if (result != FMOD_OK) {
			LM.write_log("Audio_System::playSound(): Failed to play sound. Error: ");
			LM.write_log(FMOD_ErrorString(result));
			return nullptr;
		}

		LM.write_log("Audio_System::playSound(): Successful in playing the sound object.");
		return channel;
	}

	//shutdown the core system and release all the resources
	void Audio_System::shutdown() {
		if (core_system) {
			core_system->release();
			LM.write_log("Audio_System::shutdown(): Audio_System successfully released.");
		}

		LM.write_log("Audio_System::shutdown(): Audio_system failed to release.");
	}

}

	