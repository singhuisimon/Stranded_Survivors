/**
 * @file Logic_Manager.h
 * @brief Define the logic manager class
 * @author Simon Chan (60%), Amanda Leow Boon Suan (40%)
 * @date February 2, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#ifndef LOF_LOGIC_MANAGER_H
#define LOF_LOGIC_MANAGER_H

#define LGM lof::Logic_Manager::get_instance()

#include "../Entity/Entity.h"
#include "../Utility/Constant.h"
#include "../Scripts/Script.h"
#include "../System/Logic_System.h"
#include "../Manager/Manager.h"

#include <unordered_map>
#include <vector>
#include <memory>

namespace lof {

	/**
	* @class Logic_Manager
	* @brief Defines the logic manager class.
	*/
	class Logic_Manager : public Manager {

	public:
		/**
		 * @brief Get the unique instance of Logic_Manager.
		 */
		static Logic_Manager& get_instance();

		/**
		 * @brief Initialize the logic manager
		 */
		int start_up() override; //register the script into logic system

		/**
		 * @brief Update the logic manager
		 * @param delta_time The time elapsed since the last update
		 */
		void update(float delta_time);

		/**
		 * @brief Shutdown the logic manager
		 */
		void shut_down();

	private:
		/**
		 * @brief Default constructor
		 */
		Logic_Manager() = default;
		static std::unique_ptr<Logic_Manager> instance;
		static std::once_flag once_flag;

		/**
		 * @brief Register all the scripts in the engine.
		 */
		void register_all_scripts();

	};

}

#endif