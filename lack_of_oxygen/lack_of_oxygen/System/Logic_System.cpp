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

        //if (!game_playing) {
        //    /*LM.write_log("Logic_System updating script in level editor mode");
        //    return;*/
        //}

        process_script();
    }

    void Logic_System::process_script() {
        auto& entityids = ECSM.get_entities();

        for (int i = 0; i < entityids.size(); ++i) {
            EntityID entity_id = entityids[i]->get_id();

            // Skip entities that don't have a Logic_Component
            if (!ECSM.has_component<Logic_Component>(entity_id)) {
                continue;
            }

            Logic_Component& logic = ECSM.get_component<Logic_Component>(entity_id);

            for (auto& logic_data : logic.get_logic_datas()) {
                // Skip inactive logic data
            for (auto& logic_data : logic.logic_datas) {

                // ================================= PAUSE CHECK =================================
                // Check if game is paused first
                if (GM.is_paused()) {
                    continue; // Skip this script update if game is paused
                }
                // =================================================================================

                //this is keep for in the future when level editor disable it.
                if (!logic_data->is_active) {
                    if (logic_data->state == ExecutionState::Running) {
                        logic_data->state = ExecutionState::Terminated;;
                    }
                    LM.write_log("Logic_System::update_script, entity %u logic is not active", entity_id);
                    continue;
                }
                else {
                    if (logic_data->state == ExecutionState::Terminated) {
                        logic_data->state = ExecutionState::Uninitialized;
                    }
                }

                auto script = LGM.get_script(logic_data->script_name);
                if (!script) {
                    LM.write_log("Logic_System::update_script script %s not found", logic_data->script_name.c_str());
                    return;
                }

                if (!game_playing) {
                    logic_data->state = ExecutionState::Paused;
                }

                // Handle script based on its current state
                switch (logic_data->state) {
                case ExecutionState::Uninitialized:
                    initialize_script(entity_id, logic_data, script);
                    break;

                case ExecutionState::Running:
                    update_script(entity_id, logic_data, script);
                    break;

                case ExecutionState::Paused:
                    // Future handling for completed state
                    if (game_playing) {
                        logic_data->state = ExecutionState::Running;
                    }
                    break;

                case ExecutionState::Terminated:
                    terminate_script(entity_id, logic_data, script);
                    break;

                default:
                    break;
                }
            }
        }
    }

    void Logic_System::initialize_script(EntityID entity_id, std::shared_ptr<Logic_Component::LogicData> logic_data, std::shared_ptr<Script> script) {
        //auto init_func = script->get_function(logic_data->init_func);
        if (!script->has_init()) {
            //LM.write_log("Initialization function %s not found", logic_data->init_func.c_str());
            return;
        }

        auto init_func = script->get_function("init");
        init_func(entity_id);
        logic_data->state = ExecutionState::Running;
        LM.write_log("Logic_System::initialize_script initialized script for entity %u", entity_id);
    }

    void Logic_System::update_script(EntityID entity_id, std::shared_ptr<Logic_Component::LogicData> logic_data, std::shared_ptr<Script> script) {
        //auto update_func = script->get_function(logic_data->update_func);
        if (!script->has_update()) {
            //LM.write_log("Update function %s not found", logic_data->update_func.c_str());
            return;
        }
        
        auto update_func = script->get_function("update");
        update_func(entity_id);

        // Check for game over condition to terminate the script
        if (GM.get_game_over()) {
            logic_data->state = ExecutionState::Terminated;
        }
    }

    void Logic_System::terminate_script(EntityID entity_id, std::shared_ptr<Logic_Component::LogicData> logic_data, std::shared_ptr<Script> script) {
        //auto end_func = script->get_function(logic_data->end_func);
        if (!script->has_end()) {
            //LM.write_log("End function %s not found", logic_data->end_func.c_str());
            return;
        }

        auto end_func = script->get_function("end");
        end_func(entity_id);
        logic_data->is_active = false;
        LM.write_log("Logic_System::terminate_script completed script for entity %u", entity_id);
    }

}