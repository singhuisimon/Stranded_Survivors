/**
 * @file Sound_Setting_Script.h
 * @brief Defines the script class for the Sound setting logic.
 * @details Updates the soundbar and volume settings
 * @author Amanda Leow Boon Suan (100%)
 * @date March 25, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_SOUND_SETTING_SCRIPT_H
#define LOF_SOUND_SETTING_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/ECS_Manager.h"
#include "../Manager/Game_Manager.h"
#include "../Utility/Entity_Selector_Helper.h"
#include "../Utility/Globals.h"

namespace lof {

	class Sound_Setting_Script : public Script, public std::enable_shared_from_this<Sound_Setting_Script> {
	public:
		/**
		 * @brief Constructor for Cloud_Script.
		 */
		Sound_Setting_Script();

		/**
		 * @brief Register the script by adding functions into it.
		 */
		void register_script() override;

		/**
		 * @brief Returns the script name as a string.
		 * @return The script name.
		 */
		std::string get_type() const override;

		/**
		 * @brief Initialize the bar entity.
		 */
		void init_entity();

		/**
		 * @brief Updates the sound setting .
		 */
		void update_setting();

		/**
		 * @brief Updates the multiplier for BGM SFX UI
		 */
		void update_multiplier();

		/**
		 * @brief Updates the position and width of the audio bar
		 */
		void update_audio_bar();

		/**
		 * @brief Updates the volume for the respective group
		 */
		void update_audio_group();

		/**
		 * @brief Checks if the back button is press
		 * @param entity_id The ID of the entity to check
		 */
		void check_back_press(EntityID entity_id);

	private:
		// Script name identifier
		const std::string script_name = "sound_setting_script";

		// Map holding the hover state
		std::unordered_map<std::string, bool> button_hover_states;

		/**
		 * @brief Checks if entity is the entity of interest
		 * @param entity_id The ID of the entity to check
		 */
		bool check_entity(EntityID entity_id);

		// EntityID of the important entity in sound setting
		EntityID master_id;
		EntityID bgm_id;
		EntityID sfx_id;
		EntityID back_button;

		//float values for the size and x pos and width modifier
		float master_size;
		float bgm_size;
		float sfx_size;
		float bar_x_axis;
		float x_modifier;
		float width_modifier;

	};
}
#endif // LOF_CLOUD_SCRIPT_H