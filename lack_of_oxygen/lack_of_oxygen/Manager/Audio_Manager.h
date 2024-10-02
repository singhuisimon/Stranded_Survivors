/**
 * @file Audio_Manager.h
 * @brief Declare the Audio_Manager class that functions as an interface (class wrapper) for Audio_System
 * @author Amanda Leow Boon Suan (100%)
 * @date September 27, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

// Macros for accessing manager singleton instances
#define AM  lof::Audio_Manager::get_instance()

// Include base Manager class
#include "Manager.h"

// Include necessary headers from this project
#include "../System/Audio_System.h"	//contains class Audio_System and all necessary FMOD headers
#include "../Utility/Constant.h"
#include "../Manager/Input_Manager.h"

// Include standard headers
#include <iostream>
#include <string>



namespace lof {

	//forward declaration of the Audio_System class
	class Audio_System;

	/**
	 * @class Audio_Manager
	 * @brief Acts as an interface for Audio System as well as to load,play and stop background music.
	 */
	class Audio_Manager : public Manager{
	public:

		/**
		 * @brief Get the singleton instance of the Audio_Manager.
		 * @return Reference to the Audio_Manager instance.
		 */
		static Audio_Manager& get_instance();

		/**
		 * @brief Constructor for Audio_Manager
		 */
		Audio_Manager();

		/**
		 * @brief Destructor for Audio_Manager
		 */
		~Audio_Manager();

		/**
		 * @brief Starts up the Audio System
		 * @return int Returns 0 on success, -1 on failure
		 */
		int start_up() override;

		/**
		 * @brief Updates the Audio System
		 */
		void update();

		/**
		 * @brief Shuts down the Audio System
		 */
		void shutdown();

		/**
		 * @brief Loads background music track into the Audio System
		 * @param track1_path The file path to the first background music track.
		 * @param track2_path The file path to the second background music track.
		 */
		void load_bgm(const std::string& track1_path, const std::string& track2_path);

		/**
		 * @brief Play the loaded background music track
		 * @param track_number Indicator of which track to play (1 for track1, 2 for track2)
		 */
		void play_bgm(int track_number);

		/**
		 * @brief Stops the currently playing background music.
		 */
		void stop_bgm();

	private:
		Audio_System* audio_system;		///< Pointer to the Audio_System that handles low-level audio operation
		FMOD::Sound* track1;			///< Pointer to the first background music track
		FMOD::Sound* track2;			///< Pointer to the second background music track
		FMOD::Channel* current_channel;	///< Pointer to the current playing audio channel, used to control playback(looping)
	};

}


#endif
