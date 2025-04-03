/**
 * @file Logic_Manager.h
 * @brief Define the logic manager class
 * @author Amanda Leow Boon Suan (100%)
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
		 * @brief Shutdown the logic manager
		 */
		void shut_down();

		/**
		 * @brief Add new script  to the map
		 * @param script consisting the shared ptr to the script for registering
		 */
		void add_script(std::shared_ptr<Script> script);

		/**
		 * @brief get the script from the script map
		 * @param script_name the key to obtain the script ptr from the map
		 * @return a shared ptr to the script
		 */
		std::shared_ptr<Script> get_script(const std::string& script_name);

		/**
		 * @brief Remove the script from the scripts_map
		 * @param script_name the key to obtain the script ptr that is to be removed
		 */
		void remove_script(const std::string& script_name);

		/**
		 * @brief Getter for the script name for imgui usage
		 * @return a vector of string filled with all the registered script name.
		 */
		std::vector<std::string> get_script_names();

	private:
		/**
		 * @brief Default constructor
		 */
		Logic_Manager();
		//static std::unique_ptr<Logic_Manager> instance;
		//static std::once_flag once_flag;

		/**
		 * @brief Register all the scripts in the engine.
		 */
		void register_all_scripts();

		///@ unordered map storing the name of the script as well as the script pointer itself
		std::unordered_map<std::string, std::shared_ptr<Script>> scripts_map;

	};

}

#endif