/**
 * @file Player_Tutorial_Script.h
 * @brief Define the player tutorial script class.
 * @author Liliana Hanawardani (100%)
 * @date February 13, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#ifndef LOF_PLAYER_TUTORIAL_SCRIPT_H
#define LOF_PLAYER_TUTORIAL_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Component.h"


namespace lof {

	/**
	 * @class Player_Tutorial_Script
	 * @brief Defines the player tutorial script class.
	 */
	class Player_Tutorial_Script : public Script, public std::enable_shared_from_this<Player_Tutorial_Script> {
	public:

		/**
		 * @brief Constructor for Player_Tutorial_Script.
		 * Initializes the private members of the script.
		 */
		Player_Tutorial_Script();

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
		const std::string script_name = "player_tutorial_script";

		/**
		 * @brief Controls the player tutorial movement in chair
		 */
		void player_tutorial_update() const;

	};
}

#endif 