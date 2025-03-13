/**
 * @file Cloud_Script.h
 * @brief Defines the script class for the Cloud logic.
 * @details Updates the foreground cloud movement.
 * @author Chua Wen Bin Kenny (100%)
 * @date March 11, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_CLOUD_SCRIPT_H
#define LOF_CLOUD_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {

	class Cloud_Script : public Script, public std::enable_shared_from_this<Cloud_Script> {
	public:
		/**
		 * @brief Constructor for Cloud_Script.
		 * Initializes the private members of the script.
		 */
		Cloud_Script();

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
		const std::string script_name = "cloud_script";
		float moving_speed;

		/**
		 * @brief Controls the cloud movement in the foreground
		 */
		void cloud_movement_update() const;

	};
}
#endif // LOF_CLOUD_SCRIPT_H