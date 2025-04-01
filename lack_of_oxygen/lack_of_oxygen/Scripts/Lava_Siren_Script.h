/**
 * @file Lava_Siren_Script.h
 * @brief Defines the script class for the Lava Siren playback logic.
 * @details Updates the timer and decide when lava siren gets to play.
 * @author  Amanda Leow Boon Suan (100%)
 * @date March 30, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_LAVA_SIREN_SCRIPT_H
#define LOF_LAVA_SIREN_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"
#include "../Utility/Constant.h"

namespace lof {

	class Lava_Siren_Script : public Script, public std::enable_shared_from_this<Lava_Siren_Script> {
	public:
		/**
		 * @brief Default Constructor for Lava_Siren.
		 */
		Lava_Siren_Script() = default;

		/**
		 * @brief Register the script by adding functions into it.
		 */
		void register_script() override;

		/**
		 * @brief Returns the script name as a string.
		 * @return The script name.
		 */
		std::string get_type() const override;

		void update_lava_timer(EntityID entityid);

	private:
		const std::string script_name = "lava_siren_script";
		float timer = DEFAULT_LAVA_SIREN_COUNTDOWN;
		const std::string lava_key = "lava siren";

		void set_lava_timer(float timer, EntityID entity_id);

	};
}
#endif // LOF_LAVA_SIREN_SCRIPT_H
