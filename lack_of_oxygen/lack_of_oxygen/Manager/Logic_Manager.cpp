

#include "../Scripts/Object_Moving_Script.h"
#include "../Scripts/Player_Script.h"

#include "../Manager/Logic_Manager.h"
#include "../Manager/ECS_Manager.h"

#include <mutex>

namespace lof {

	std::unique_ptr<Logic_Manager> Logic_Manager::instance;
	std::once_flag Logic_Manager::once_flag;

	Logic_Manager& Logic_Manager::get_instance() {
		std::call_once(once_flag, []() {
			instance.reset(new Logic_Manager);
			});
		return *instance;
	}

	int Logic_Manager::start_up() {
		if (is_started()) {
			//LM.write_log("Logic_Manager::start_up(): Logic_Manager is already started.");
			return 0; //Already started
		}

		try {
			register_all_scripts();
			///LM.write_log("Logic_Manager::start_up(): Added script """);
		}
		catch (const std::exception& e) {
			//LM.write_log("Logic_Manager::start_up(): Failed to initalize. Error: %s", e.what());
			return -1;
		}

		m_is_started = true;
		return 0;
	}

	void Logic_Manager::update(float delta_time) {

		//LM.write_log("Logic_Manager:: updating");
		LGS.update_script(delta_time);
	}

	void Logic_Manager::register_all_scripts() {
		// Register Object Moving Script
		Object_Moving_Script object_moving_script;
		object_moving_script.register_script();
		//LM.write_log("Registered Object Moving Script");

		Player_Script player_script;
		player_script.register_script();
		//LM.write_log("Registered Player Script");

		// Register other scripts here as they are added.
	}

	void Logic_Manager::shut_down() {
		if (!is_started()) {
			//LM.write_log("Logic_Manager::shut_down(): Logic_Manager is not started. Nothing to shut down");
			return;
		}

		LGS.cleanup();
		if (instance) {
			instance.reset();
			//LM.write_log("Logic_Manager::shut_down(): Logic_Manager instance deleted.");
		}

		//LM.write_log("Logic_Manager::shut_down(): Logic_Manager shut down.");
		
	}
}