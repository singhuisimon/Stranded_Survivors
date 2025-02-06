

#include "../Scripts/Object_Moving_Script.h"
#include "../Scripts/Player_Script.h"
#include "../Scripts/Mining_Script.h"

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

		//LM.write_log("Logic_Manager:: updating");
		LGS.update_script(delta_time);
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

		//clear anything here ig :">

		LGS.cleanup();
		if (instance) {
			instance.reset();
			LM.write_log("Logic_Manager::shut_down(): Logic_Manager instance deleted.");
		}

		LM.write_log("Logic_Manager::shut_down(): Logic_Manager shut down.");
		
	}

	






	// Loop over all entities and update their respective logic states
//const auto& entityids = get_entities();

//for (EntityID entityid : entityids) {
//	if (!ECSM.has_component<Logic_Component>(entityid)) {
//		continue;
//	}

//	Logic_Component& logic = ECSM.get_component<Logic_Component>(entityid);
//	std::string state = get_state(entityid);
//	auto script = Logic_System::get_instance().get_script(logic.script_name);

//	if (script) {
//		auto update_func = script->get_function(logic.update_func);
//		if (update_func) {
//			// Here, you can use state logic to determine which function to call
//			update_func(&entityid);
//		}
//	}
//}

	/*Logic_Manager& Logic_Manager::get_instance() {
		static Logic_Manager instance;
		return instance;
	}

	void Logic_Manager::add_script(Entity* entity, std::unique_ptr<Logic_System> scripts) {
		active_scripts[entity].push_back(std::move(scripts));
	}

	void Logic_Manager::remove_script(Entity* entity, const LogicSystemType& script_name) {
		auto& scripts = active_scripts[entity];
		scripts.erase(std::remove_if(scripts.begin(), scripts.end(),
			[&script_name](const std::unique_ptr<Logic_System>& script) {
				return script->get_script() == script_name;
			}), scripts.end());
	}

	void Logic_Manager::update(float delta_time) {
		for (auto& [entity, scripts] : active_scripts) {
			for (auto& script : scripts){
				script->update_script(entity, delta_time);
			}
		}
	}*/


	//TODO:: DOUBLE CHECK AND RESOLVE THE ERROR IT COMES OUT WHEN I UNCOMMENT LINE 27 IN THE FILE
	// MAKE SURE TO DOUBLE CHECK HOW OBJECT MOVING SCRIPT IS, ASSIGNING SCRIPT , ETC
}