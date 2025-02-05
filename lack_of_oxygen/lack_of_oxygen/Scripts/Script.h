#pragma once
#ifndef LOF_SCRIPT_H
#define LOF_SCRIPT_H

#include <functional>
#include <string>
#include <unordered_map>
#include <iostream>

#include "../Entity/Entity.h"
#include "../System/Logic_System.h"

//typedef std::function<void(EntityID*)> Init_Script;
//typedef std::function<void(EntityID*)> Update_Script;
//typedef std::function<void(EntityID*)> End_Script;

namespace lof {

	class Script {
	public:

		// Using std::function instead of raw function pointers for flexibility
		using ScriptFunction = std::function<void(EntityID)>;

		Script() = default;

		Script(const std::string& name) : script_name(name) {}

		//this is to load data from the logic component and save into it as well (check if needed)
		//virtual void save_logic_data(ScriptData* data) = 0;
		//virtual void load_logic_data(ScriptData* data) = 0;

		void add_function(const std::string& function_name, ScriptFunction func) {
			functions_map[function_name] = func;
		}

		ScriptFunction get_function(const std::string& function_name) const {
			auto it = functions_map.find(function_name);
			if (it != functions_map.end()) {
				LM.write_log("FOUND FUNCTION %s", function_name.c_str());
				return it->second;
			}
			LM.write_log("DID NOT FIND FUNCTION %s", function_name.c_str());
			return nullptr;
		}

		const std::string& get_name() const { return script_name; }

	private:

		std::string script_name;

		//using init, update, end name in logic component retrieve the function
		std::unordered_map<std::string, ScriptFunction> functions_map;

	};
}

#endif

/*void update_scripts(EntityID* entity) {
			if (update_script) {
				update_script(entity);
			}
		}*/


/*Script(const std::string& name, ScriptFunction init, ScriptFunction update, ScriptFunction end)
	: script_name(name), init_script(init),
	update_script(update), end_script(end){}*/

//struct Scripting {
	//	Init_Script init_script;
	//	Update_Script update_script;
	//	End_Script end_script;
	//};

/*void run_init(EntityID* entity) {
			if (scripting.init_script) scripting.init_script(entity);
		}

		void run_update(EntityID* entity) {
			if (scripting.update_script) scripting.update_script(entity);
		}

		void run_end(EntityID* entity) {
			if (scripting.end_script) scripting.end_script(entity);
		}*/


		//virtual ~Script() = default;