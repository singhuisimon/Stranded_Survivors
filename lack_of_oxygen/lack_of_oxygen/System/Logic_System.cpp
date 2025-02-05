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

    std::string Logic_System::get_type() const {
        return "Logic_System";
    }

    void Logic_System::update(float delta_time) {
        // Loop over each script in the map and update
        //loop over the entity
        //check for those that have logic component
        //after that loop to get the script name and ensure to update or based off entity state to call upon the script function
        
        const auto& entityids = get_entities();
        
        for (EntityID entityid : entityids) {
            if (!ECSM.has_component<Logic_Component>(entityid)) {
                continue;
            }

            Logic_Component& logic = ECSM.get_component<Logic_Component>(entityid);

            for (auto& logic_data : logic.logic_datas) {
                if (!logic_data->is_active) {
                    continue;
                }
                auto script = get_script(logic_data->script_name);
                if (!script) {
                    continue;
                }

                if (logic_data->state == ExecutionState::Uninitialized) {
                    auto init_func = script->get_function(logic_data->init_func);
                    if (init_func) {
                        init_func(&entityid);
                    }
                    logic_data->state = ExecutionState::Running;
                }
                else if (logic_data->state == ExecutionState::Running) {
                    auto update_func = script->get_function(logic_data->update_func);
                    if (update_func) {
                        update_func(&entityid);
                    }
                }
                else if (logic_data->state == ExecutionState::Completed) {
                    auto end_func = script->get_function(logic_data->end_func);
                    if (end_func) {
                        end_func(&entityid);
                    }
                    //logic_data->is_active = false; //maybe add a check on
                    //if state == completed & still active change to running or waiting?
                }

                //create a function here to check what state the entity is in to play the
                    //respective required function.
                    //from there then optain the function 
                    //script->execute(entity_id, logic_data.script_data);
                /*auto update_func = script->get_function(logic_data->update_func);
                if (update_func) {
                    update_func(&entityid);
                }*/
            }
        }
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

    //static bool first_frame = true;

        //for (EntityID entity_id : get_entities()) {
        //    auto& logic = ECSM.get_component<Logic_Component>(entity_id);
        //    auto* entity = ECSM.get_entity(entity_id);

        //    if (logic.is_active && !level_editor_mode) {
        //        //update all scripts attached to this entity
        //        for (auto& [category, scripts] : logic.scripts) {
        //            for (auto& [script_name, script_data] : scripts) {
        //                update_scripts(logic, script_name, delta_time);
        //            }
        //        }
        //    }

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