/**
 * @file Tutorial_Lights_Script.h
 * @brief Define the Tutorial_Lights_Script class
 * @author Amanda Leow Boon Suan (100%)
 * @date March 13, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_TUTORIAL_LIGHTS_SCRIPT_H
#define LOF_TUTORIAL_LIGHTS_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"
#include "../System/Particle_System.h"


// 64 pixels/sec (Speed)

namespace lof {
	class Tutorial_Light_Script : public Script, public std::enable_shared_from_this<Tutorial_Light_Script> {
	public:
		
		int current_frame_index = 0; //current frame that the texture is at
		int elapsed_frames = 0;	//tracks how many frames have passed
		std::string base_texture = "Siren_Bulb_Batch_14_";
		std::string audio_name = "tutorial_light";

		/**
		* @brief The constructor of the tutorial light script
		*/
		Tutorial_Light_Script();

		/**
		* @brief Register the script as well as its functions for init update
		*/
		void register_script() override;

		/**
		* @brief Get the script name
		* @return std::string value containing the script name
		*/
		std::string get_type() const override;

		/**
		* @brief Initialize the current frame for the lights
		*/
		void init_light();

		/**
		* @brief Update the frame for the lights
		*/
		void update_frame();

	private:
		//the script name
		const std::string script_name = "tutorial_light_script";

		//set the audio cooldown for the siren 0.4s
		float siren_cooldown = SIREN_AUDIO_COOLDOWN;

		/**
		* @brief Update the light animation
		*/
		void lights_animation();

		/**
		* @brief Update the light sound
		*/
		void play_siren_audio();
		

	};
}

#endif 