/**
 * @file Logic_System.cpp
 * @brief Defines the definition for Logic systems.
 * @author Amanda Leow Boon Suan (80%), Simon Chan (5%), Wai Lwin Thit (5%)
 * @date November 07, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

// Include the header file
#include <memory>
#include <string>
#include <functional>
#include <mutex>

#include "../Scripts/Script.h"

#include "Logic_System.h"

// Include the manager headers
#include "../Manager/ECS_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Logic_Manager.h"
#include "../Manager/Game_Manager.h"
#include "../Utility/Globals.h"

namespace lof {

    std::unique_ptr<Logic_System> Logic_System::instance;
    std::once_flag Logic_System::once_flag;

    Logic_System& Logic_System::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Logic_System);
        });
        return *instance;
    }

    Logic_System::Logic_System() {
        signature.set(ECSM.get_component_id<Logic_Component>());
    }

    Logic_System::~Logic_System() {

        // Clean up
        cleanup();

        if (instance) {
            instance.reset();
			LM.write_log("Logic_System::instance reset");
        }
    }

    std::string Logic_System::get_type() const {
        return "Logic_System";
    }

    void Logic_System::update(float delta_time) {
        (void)delta_time;
        //keeping this empty
    }

    void Logic_System::update_script() {
        // Loop over each script in the map and update
        //loop over the entity
        //check for those that have logic component
        //after that loop to get the script name and ensure to update or based off entity state to call upon the script function
        
        //if (level_editor_mode) {
        //    //LM.write_log("Logic_System updating script in level editor mode");
        //    return;
        //}

        if (!game_playing) {
            //LM.write_log("Logic_System updating script in level editor mode");
            return;
        }

        auto& entityids = ECSM.get_entities();

        for (int i = 0; i < entityids.size(); ++i) {


            EntityID entityid = entityids[i]->get_id();

            if (!ECSM.has_component<Logic_Component>(entityid)) {
                //LM.write_log("Logic_System::update_script, entity %u has no logic component", entityid);
                continue;
            }
            //LM.write_log("Logic_System::update_script, entity %u has logic component", entityid);

            Logic_Component& logic = ECSM.get_component<Logic_Component>(entityid);

            for (auto& logic_data : logic.logic_datas) {

                //this is keep for in the future when level editor disable it.
                if (!logic_data->is_active) {
                    LM.write_log("Logic_System::update_script, entity %u logic is not active", entityid);
                    continue;
                }

                //gets the script based off the script name
                auto script = get_script(logic_data->script_name);
                if (!script) {
                    LM.write_log("Logic_System::update_script script not found");
                    continue;
                }

                if (logic_data->state == ExecutionState::Uninitialized) {
                    auto init_func = script->get_function(logic_data->init_func);
                    if (!init_func) {
                        return;
                    }
                    init_func(entityid);
                    logic_data->state = ExecutionState::Running;
                    //LM.write_log("Logic_System::update_script initializing script");
                }
                else if (logic_data->state == ExecutionState::Running) {
                    auto update_func = script->get_function(logic_data->update_func);
                    if (!update_func) {
                        return;
                    }
                    update_func(entityid);
                    //LM.write_log("Logic_System::update_script updating script");
					if (GM.get_game_over()) {
						logic_data->state = ExecutionState::Terminated;
					}
                }
                else if (logic_data->state == ExecutionState::Completed) {
                    //LM.write_log("Logic_System::update_script completed script");
                    //if state == completed & still active change to running or waiting?
                    //add condition TODO IN M5
                }

                if (logic_data->state == ExecutionState::Terminated) {
                    auto end_func = script->get_function(logic_data->end_func);
                    if (!end_func) {
                        return;
                    }
                    end_func(entityid);
                    logic_data->is_active = false;
                    LM.write_log("Logic_System::update_script completed script");
                }
            }
        }
    }

    void Logic_System::cleanup() {
        for (auto& script_pair : script_map) {
            script_pair.second.reset(); // reset each shared_ptr
            LM.write_log("Logic_System::cleanup() resetting shared_ptr");
        }

        script_map.clear();
		
    }

    void Logic_System::add_script(const std::string& script_name, std::shared_ptr<Script> script) {
        script_map[script_name] = script;
        LM.write_log("registered %s script into script map", script_name.c_str());
    }

    std::shared_ptr<Script> Logic_System::get_script(const std::string& script_name) {

        auto it = script_map.find(script_name);
        if (it != script_map.end()) {
            return it->second.lock();
        }
        return nullptr;
    }

	void Logic_System::remove_script(const std::string& script_name) {
		auto it = script_map.find(script_name);
		if (it != script_map.end()) {
			script_map.erase(it);
		}
	}

}