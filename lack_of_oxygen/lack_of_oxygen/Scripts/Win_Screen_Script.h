/**
 * @file Win_Screen_Script.h
 * @brief Defines the script class for the Win Screen animation logic.
 * @details Updates the animation and set up for win screen.
 * @author Chua Wen Bin Kenny (100%)
 * @date March 29, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_WIN_SCREEN_SCRIPT_H
#define LOF_WIN_SCREEN_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {

	class Win_Screen_Script : public Script, public std::enable_shared_from_this<Win_Screen_Script> {
	public:
		/**
		 * @brief Constructor for Win_Screen_Script.
		 * Initializes the private members of the script.
		 */
		Win_Screen_Script();

		/**
		 * @brief Register the script by adding functions into it.
		 */
		void register_script() override;

		/**
		 * @brief Returns the script name as a string.
		 * @return The script name.
		 */
		std::string get_type() const override;

	private:

		// Script Name
		const std::string script_name = "win_screen_script";

		// Ship and win background name
		const std::string ship_name = "ship";
		const std::string win_background_name = "win_background";
		const std::string restart_button_name = "restart_button";
		const std::string main_menu_button_name = "main_menu_button";

		// Camera top and bottom ui name
		const std::string top_camera_ui = "top_camera_ui";
		const std::string bottom_camera_ui = "bottom_camera_ui";

		// Cap for top and bottom UI
		const float top_cap = 420.0f;
		const float bottom_cap = -630.0f;

		// Ship's parameters
		float ship_launching_speed;
		float launch_duration;

		// Win Background parameters
		float background_speed;
		bool background_landed;		// Flag for win background landing in camera
		float time_landed;			// Track time left in launch duration when win background landed 

		//Audio boolean
		bool win_audio_played;

		/**
		 * @brief Controls the cloud movement in the foreground
		 */
		void win_screen_update();

	};
}
#endif // LOF_WIN_SCREEN_SCRIPT_H