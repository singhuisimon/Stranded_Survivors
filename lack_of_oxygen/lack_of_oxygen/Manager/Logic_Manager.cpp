/**
 * @file Logic_Manager.cpp
 * @brief Declare the logic manager class
 * @author Amanda Leow Boon Suan (80%), Simon Chan (20%)
 * @date February 2, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "../Scripts/Object_Moving_Script.h"
#include "../Scripts/Player_Script.h"
#include "../Scripts/Mining_Script.h"
#include "../Scripts/Cloud_Script.h"
#include "../Scripts/Tutorial_Script.h"
#include "../Scripts/Tutorial_Lights_Script.h"
#include "../Scripts/Player_Tutorial_Script.h"
#include "../Scripts/Mini_Map_Script.h"

#include "../Manager/Logic_Manager.h"
#include "../Manager/ECS_Manager.h"

#include <mutex>

namespace lof {

	std::unique_ptr<Logic_Manager> Logic_Manager::instance;
	std::once_flag Logic_Manager::once_flag;

	Logic_Manager::Logic_Manager() {
		set_type("Logic_Manager");
	}

	Logic_Manager& Logic_Manager::get_instance() {
		std::call_once(once_flag, []() {
			instance.reset(new Logic_Manager);
			});
		return *instance;
	}

	int Logic_Manager::start_up() {
		if (is_started()) {
			LM.write_log("Logic_Manager::start_up(): Logic_Manager is already started.");
			return 0; //Already started
		}

		try {
			register_all_scripts();
			LM.write_log("Logic_Manager::start_up(): Added script """);
		}
		catch (const std::exception& e) {
			LM.write_log("Logic_Manager::start_up(): Failed to initalize. Error: %s", e.what());
			return -1;
		}

		m_is_started = true;
		return 0;
	}

	void Logic_Manager::register_all_scripts() {

		scripts_map.clear();

		//add the script here
		add_script(std::make_shared<Object_Moving_Script>());
		add_script(std::make_shared<Player_Script>());
		add_script(std::make_shared<Mining_Script>());
		add_script(std::make_shared<Cloud_Script>());
		add_script(std::make_shared<Tutorial_Script>());
		add_script(std::make_shared<Tutorial_Light_Script>());
		add_script(std::make_shared<Player_Tutorial_Script>());
		add_script(std::make_shared<Mini_Map_Script>());
		LM.write_log("Logic_Manager::register_all_scripts(): Registered built-in scripts.");

		// Register other scripts here as they are added.
	}

	void Logic_Manager::shut_down() {
		if (!is_started()) {
			LM.write_log("Logic_Manager::shut_down(): Logic_Manager is not started. Nothing to shut down");
			return;
		}

		scripts_map.clear();
		m_is_started = false;

		//LGS.cleanup();
		if (instance) {
			instance.reset();
			LM.write_log("Logic_Manager::shut_down(): Logic_Manager instance deleted.");
		}

		LM.write_log("Logic_Manager::shut_down(): Logic_Manager shut down.");
		
	}

	void Logic_Manager::add_script(std::shared_ptr<Script> script) {
		if (!script) return;
		scripts_map[script->get_type()] = script;
		script->register_script();
		LM.write_log("Logic_Manager::add_script(): Registered script '%s'", script->get_type().c_str());
	}

	std::shared_ptr<Script> Logic_Manager::get_script(const std::string& script_name) {
		auto it = scripts_map.find(script_name);
		if (it != scripts_map.end()) {
			//LM.write_log("Logic_Manager::get_script(): Script '%s' found", script_name.c_str());
			return it->second;
		}
		LM.write_log("Logic_Manager::get_script(): Script '%s' not found", script_name.c_str());
		return nullptr;
	}

	void Logic_Manager::remove_script(const std::string& script_name) {
		if (scripts_map.erase(script_name)) {
			LM.write_log("Logic_Manager::remove_script(): Removed script '%s'", script_name.c_str());
		}
	}

	std::vector<std::string> Logic_Manager::get_script_names() {
		std::vector<std::string> script_names;
		for (const auto& pair : scripts_map) {
			script_names.push_back(pair.first);
		}
		return script_names;
	}
}