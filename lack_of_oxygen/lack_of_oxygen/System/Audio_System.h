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

namespace lof {

	/**
	 * @class Audio_System
	 * @brief Handles the initialization, playback and shutdown of audio using FMOD.
	 *		  It is also responsible for loading sound, playing osund and updating the FMOD system.
	 */
	class Audio_System {
	public:

		/**
		 * @brief Constructor for the Audio_System Class
		 */
		Audio_System();

		/**
		 * @brief Destructor for the Audio_System Class
		 */
		~Audio_System();

		/**
		 * @brief Initializes the FMOD core system for audio playback.
		 * @return True if initialization is successful, otherwise false.
		 */
		bool initialize();

		/**
		 * @brief Updates the FMOD system
		 */
		void update();

		/**
		 * @brief Shuts down the FMOD system, releasing any loaded sound and resources.
		 *	      Ensures that all sounds are stopped and the system is safely closed.
		 */
		void shutdown();

		/**
		 * @brief Loads a sound from the specified file_path into the FMOD system
		 * @param file_path The path to the audio file (current format .wav)
		 * @return A pointer to the loaded FMOD::Sound Object, or nullptr if loading fails.
		 */
		FMOD::Sound* load_sound(const std::string& file_path);

		/**
		 * @brief Plays the provided FMOD::Sound object.
		 * @param sound A pointer to the FMOD::Sound object.
		 * @param loop If true, sound will loop indefinitely, otherwise if false sound will only play once
		 *		  It is defaulted to be false.
		 * @return A pointer to the FMOD::Channel object controlling the playback of the sound.
		 */
		FMOD::Channel* play_sound(FMOD::Sound* sound, bool loop = false);

	private:
		FMOD::System* core_system; ///< Pointer to the FMOD core system responsible for managing audio playback
	};

}

#endif

