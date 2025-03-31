/**
 * @file Tutorial_Script.h
 * @brief Define the Tutorial_Script class
 * @author Amanda Leow Boon Suan (100%)
 * @date March 10, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_TUTORIAL_SCRIPT_H
#define LOF_TUTORIAL_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {
	class Tutorial_Script : public Script, public std::enable_shared_from_this<Tutorial_Script> {
	public:
		/**
		* @brief The default constructor of the tutorial script class
		*/
		Tutorial_Script() = default;

		/**
		* @brief Register the script and its functions
		*/
		void register_script() override;

		/**
		* @brief Get the script name
		* @return std::string containing the script name
		*/
		std::string get_type() const override;

		/**
		* @brief Check and update the keys
		*/
		void check_keys();

		/**
		* @brief Check if the key is just pressed 
		* @param key The GLFW key to check
		* @return bool value containing if the key just pressed or not
		*/
		bool is_key_just_pressed(int key);

		/**
		* @brief Check if the key is just released
		* @param key the GLFW key to check
		* @return bool value containing if the key just released or not
		*/
		bool is_key_just_released(int key);

	private:

		//the map on whether the tutorial sound is being played
		std::unordered_map<std::string, bool> tutorial_button_playing;
		//the map on the previous state of the keys
		std::unordered_map<int, bool> key_previous_state;

		//the current tutorial page. current it goes from 1 to 10.
		int tutorial_page = 1;

		//the script name
		const std::string script_name = "tutorial_script";

		//the boolean values for the keys
		bool key_a_pressed = false;
		bool key_d_pressed = false;
		bool key_e_pressed = false;
		bool key_a_last_frame = false;
		bool key_d_last_frame = false;
		bool key_e_last_frame = false;
		bool key_backslash_pressed = false;
		bool key_backslash_last_frame = false;

		//boolean values for transiting to play scene
		bool transitioning = false;

		/**
		* @brief Set the tutorial page
		* @param new_page_num the new page num to set for the tutorial page
		*/
		void set_tutorial_page(int new_page_num);

		/**
		* @brief The default constructor of the tutorial script class
		* @param new_bool the new boolean value to set for transitioning
		*/
		void set_transitioning(bool new_bool);

		/**
		* @brief Getter for the transitioning
		* @return boolean value of the transitioning
		*/
		bool get_transitioning();

		/**
		* @brief Update the button visibility
		* @param entity_id the button entity
		* @param graphic_comp the graphic component of the entity_id
		*/
		void update_button_visibility(EntityID entity_id, Graphics_Component& graphic_comp);

		/**
		* @brief Update the text visibility for next and previous
		*/
		void update_text_visibility();

		/**
		* @brief Check if the buttons are pressed from the keys
		* @param entity_id The button entity to check
		*/
		void check_pressing_button(EntityID entity_id);

		/**
		* @brief Containing the codes to go next scene.
		*/
		void transit_next_scene();

	};
}

#endif 