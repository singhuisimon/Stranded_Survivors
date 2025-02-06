/**
 * @file Logic_System.cpp
 * @brief Defines the definition for Logic systems.
 * @author Simon Chan (100%)
 * @date November 07, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

// Include the header file
#include <memory>
#include <string>
#include <functional>

#include "../Scripts/Script.h"

#include "Logic_System.h"

// Include the manager headers
#include "../Manager/ECS_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Manager/Logic_Manager.h"
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
        }
    }

    std::string Logic_System::get_type() const {
        return "Logic_System";
    }

    void Logic_System::update(float delta_time) {
        //keeping this empty
    }

    void Logic_System::update_script(float delta_time) {
        // Loop over each script in the map and update
        //loop over the entity
        //check for those that have logic component
        //after that loop to get the script name and ensure to update or based off entity state to call upon the script function
        
        if (level_editor_mode) {
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
            LM.write_log("Logic_System::update_script, entity %u has logic component", entityid);

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
                    LM.write_log("Logic_System::update_script initializing script");
                }
                else if (logic_data->state == ExecutionState::Running) {
                    auto update_func = script->get_function(logic_data->update_func);
                    if (!update_func) {
                        return;
                    }
                    update_func(entityid);
                    LM.write_log("Logic_System::update_script updating script");
                }
                else if (logic_data->state == ExecutionState::Completed) {
                    auto end_func = script->get_function(logic_data->end_func);
                    if (!end_func) {
                        return;
                    }
                    end_func(entityid);
                    LM.write_log("Logic_System::update_script completed script");
                    //logic_data->is_active = false; //maybe add a check on
                    //if state == completed & still active change to running or waiting?
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
    }

    std::shared_ptr<Script> Logic_System::get_script(const std::string& script_name) {
        auto it = script_map.find(script_name);
        if (it != script_map.end()) {
            return it->second;
        }
        return nullptr;
    }

    //Logic_System::~Logic_System() {
    //    // Clean up
    //    for (auto& [name, script] : script_map) {
    //        delete script;  // Make sure to delete dynamically allocated scripts
    //    }
    //}

            // First frame debugging
            //if (first_frame) {
            //    LM.write_log("Entity [%d] '%s' - Pattern: %d, Active: %d",
            //        entity_id,
            //        entity->get_name().c_str(),
            //        static_cast<int>(logic.movement_pattern),
            //        logic.is_active);
            //}

            //// Store origin position if not set
            //if (length_vec2d(logic.origin_pos) == 0) {
            //    logic.origin_pos = transform.position;
            //    LM.write_log("Set origin for entity [%d] '%s' to (%.2f, %.2f)",
            //        entity_id,
            //        entity->get_name().c_str(),
            //        logic.origin_pos.x,
            //        logic.origin_pos.y);
            //}

            //if (logic.is_active && !level_editor_mode) {
            //    logic.timer += delta_time;

            //    // Debug every second
            //    static float debug_timer = 0.0f;
            //    debug_timer += delta_time;
            //    if (debug_timer >= 1.0f) {
            //        LM.write_log("Entity [%d] '%s' - Pattern: %d, Timer: %.2f, Pos: (%.2f, %.2f)",
            //            entity_id,
            //            entity->get_name().c_str(),
            //            static_cast<int>(logic.movement_pattern),
            //            logic.timer,
            //            transform.position.x,
            //            transform.position.y);
            //        debug_timer = 0.0f;
            //    }

                //update_movement(logic, transform, delta_time);

                //switch (logic.movement_pattern) {
                //case Logic_Component::MovementPattern::LINEAR:
                //    //update_linear_movement(logic, transform, delta_time);
                //    break;

                //case Logic_Component::MovementPattern::CIRCULAR:
                //    //update_circular_movement(logic, transform, delta_time);
                //    break;

                //default:
                //    LM.write_log("WARNING: Unknown movement pattern %d for entity [%d] '%s'",
                //        static_cast<int>(logic.movement_pattern),
                //        entity_id,
                //        entity->get_name().c_str());
                //    break;
                //}
            //}
        //}
}