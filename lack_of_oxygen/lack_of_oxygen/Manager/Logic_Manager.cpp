/**
 * @file Logic_Manager.cpp
 * @brief Declare the logic manager class
 * @author Simon Chan (60%), Amanda Leow Boon Suan (40%)
 * @date February 2, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "../Scripts/Object_Moving_Script.h"
#include "../Scripts/Player_Script.h"
#include "../Scripts/Mining_Script.h"

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

	void Logic_Manager::update(float delta_time) {
		(void)delta_time;
		//LM.write_log("Logic_Manager:: updating");
		LGS.update_script();
	}

	void Logic_Manager::register_all_scripts() {
		// Register Object Moving Script
		Object_Moving_Script object_moving_script;
		object_moving_script.register_script();
		LM.write_log("Registered Object Moving Script");

		Player_Script player_script;
		player_script.register_script();
		LM.write_log("Registered Player Script");

		Mining_Script mining_script;
		mining_script.register_script();
		LM.write_log("Registered Mining Script");

		// Register other scripts here as they are added.
	}

	void Logic_Manager::shut_down() {
		if (!is_started()) {
			LM.write_log("Logic_Manager::shut_down(): Logic_Manager is not started. Nothing to shut down");
			return;
		}

		LGS.cleanup();
		if (instance) {
			instance.reset();
			LM.write_log("Logic_Manager::shut_down(): Logic_Manager instance deleted.");
		}

		LM.write_log("Logic_Manager::shut_down(): Logic_Manager shut down.");
		
	}
}