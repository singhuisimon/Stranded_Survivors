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

	class Logic_Manager : public Manager {

	public:
		static Logic_Manager& get_instance();

		int start_up() override; //register the script into logic system

		void update(float delta_time);

		void shut_down(); //shutdown aka save all the modified script data maybe? idk:">

	private:
		Logic_Manager() = default;
		static std::unique_ptr<Logic_Manager> instance;
		static std::once_flag once_flag;

		// Helper function to register all available scripts
		void register_all_scripts();

	};

	/*class Logic_Manager : public Manager {
	private:
		std::unordered_map<Entity*, std::vector<std::unique_ptr<Logic_System>>> active_scripts;

		Logic_Manager() = default;

	public:
		static Logic_Manager& get_instance();

		void add_script(Entity* entity, std::unique_ptr<Logic_System> scripts);

		void remove_script(Entity* entity, const LogicSystemType& script_name);

		void update(float delta_time);

		Logic_Manager(const Logic_Manager&) = delete;
		Logic_Manager& operator=(const Logic_Manager&) = delete;

		~Logic_Manager() = default;
	};*/

	/*void set_state(EntityID entityid, const std::string& script, const std::string& state);

	std::string get_state(EntityID entityid) const;*/

	//see if there is a way i can do enum for all the script essentially 1 entity can have multiple script
		//with each script having it's own state if possible
		//std::unordered_map<EntityID, std::unordered_map<std::string, std::string>> entity_states; 

}

#endif