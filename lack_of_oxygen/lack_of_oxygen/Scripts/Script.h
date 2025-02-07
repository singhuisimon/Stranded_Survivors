/**
 * @file Script.h
 * @brief Base Class for all scripts in the game.
 * @author Amanda Leow Boon Suan (100%)
 * @date February 3, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#ifndef LOF_SCRIPT_H
#define LOF_SCRIPT_H

#include <functional>
#include <string>
#include <unordered_map>
#include <iostream>

#include "../Entity/Entity.h"
#include "../System/Logic_System.h"

/**
* @class Script
* @brief Base class for the scripts in the game.
*/
namespace lof {

	class Script {
	public:

		// Using std::function instead of raw function pointers for flexibility
		using ScriptFunction = std::function<void(EntityID)>;

		/**
		 * @brief Default constructor
		 */
		Script() = default;

		/**
		 * @brief Destructor
		 */
		virtual ~Script() {
			functions_map.clear();
		}

		/**
		 * @brief Constructor with script name
		 * @param name Name of the script
		 */
		Script(const std::string& name) : script_name(name) {}

		/**
		 * @brief Register the script
		 */
		virtual void register_script() = 0;

		/**
		 * @brief Add function to the functions_map
		 * @param function_name Name of the function
		 * @param func Function to be added
		 */
		void add_function(const std::string& function_name, ScriptFunction func) {
			functions_map[function_name] = func;
		}

		/**
		* @brief Get the function from the functions_map
		* @param function_name Name of the function
		*/
		ScriptFunction get_function(const std::string& function_name) const {
			auto it = functions_map.find(function_name);
			if (it != functions_map.end()) {
				//LM.write_log("FOUND FUNCTION %s", function_name.c_str());
				return it->second;
			}
			LM.write_log("DID NOT FIND FUNCTION %s", function_name.c_str());
			return nullptr;
		}

		/**
		 * @brief Get the name of the script
		 * @return Name of the script
		 */
		const std::string& get_name() const { return script_name; }

	private:

		// Name of the script
		std::string script_name;

		//using init, update, end name in logic component retrieve the function
		std::unordered_map<std::string, ScriptFunction> functions_map;

	};
}

#endif