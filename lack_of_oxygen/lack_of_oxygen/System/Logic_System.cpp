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
#include "Logic_System.h"

// Include the manager headers
#include "../Manager/ECS_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Utility/Globals.h"

namespace lof {
    Logic_System::Logic_System() {
        signature.set(ECSM.get_component_id<Logic_Component>());
        signature.set(ECSM.get_component_id<Transform2D>());
    }

    std::string Logic_System::get_type() const {
        return "Logic_System";
    }

    void Logic_System::update(float delta_time) {
        static bool first_frame = true;

        for (EntityID entity_id : get_entities()) {
            auto& logic = ECSM.get_component<Logic_Component>(entity_id);
            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto* entity = ECSM.get_entity(entity_id);

            // First frame debugging
            if (first_frame) {
                LM.write_log("Entity [%d] '%s' - Pattern: %d, Active: %d",
                    entity_id,
                    entity->get_name().c_str(),
                    static_cast<int>(logic.movement_pattern),
                    logic.is_active);
            }

            // Store origin position if not set
            if (length_vec2d(logic.origin_pos) == 0) {
                logic.origin_pos = transform.position;
                LM.write_log("Set origin for entity [%d] '%s' to (%.2f, %.2f)",
                    entity_id,
                    entity->get_name().c_str(),
                    logic.origin_pos.x,
                    logic.origin_pos.y);
            }

            if (logic.is_active){//} && !level_editor_mode) {
                logic.timer += delta_time;

                // Debug every second
                static float debug_timer = 0.0f;
                debug_timer += delta_time;
                if (debug_timer >= 1.0f) {
                    LM.write_log("Entity [%d] '%s' - Pattern: %d, Timer: %.2f, Pos: (%.2f, %.2f)",
                        entity_id,
                        entity->get_name().c_str(),
                        static_cast<int>(logic.movement_pattern),
                        logic.timer,
                        transform.position.x,
                        transform.position.y);
                    debug_timer = 0.0f;
                }

                switch (logic.movement_pattern) {
                case Logic_Component::MovementPattern::LINEAR:
                    update_linear_movement(logic, transform, delta_time);
                    break;

                case Logic_Component::MovementPattern::CIRCULAR:
                    update_circular_movement(logic, transform, delta_time);
                    break;

                default:
                    LM.write_log("WARNING: Unknown movement pattern %d for entity [%d] '%s'",
                        static_cast<int>(logic.movement_pattern),
                        entity_id,
                        entity->get_name().c_str());
                    break;
                }
            }
        }

        first_frame = false;
    }

    void Logic_System::update_linear_movement(Logic_Component& logic, Transform2D& transform, float delta_time) {
        float t = logic.movement_speed * logic.timer;

        if (!logic.reverse_direction) {
            // Horizontal movement
            float new_x = logic.origin_pos.x + logic.movement_range * sin(t);
            transform.position.x = new_x;
        }
        else {
            // Vertical movement
            float new_y = logic.origin_pos.y + logic.movement_range * sin(t);
            transform.position.y = new_y;
        }
    }

    void Logic_System::update_circular_movement(Logic_Component& logic, Transform2D& transform, float delta_time) {
        float t = logic.movement_speed * logic.timer;

        // Debug the circular motion parameters
        static float debug_timer = 0.0f;
        debug_timer += delta_time;
        if (debug_timer >= 1.0f) {
            LM.write_log("Circular Movement - Time: %.2f, Origin: (%.2f, %.2f)",
                t, logic.origin_pos.x, logic.origin_pos.y);
            debug_timer = 0.0f;
        }

        // Calculate new position using parametric equations of a circle
        float new_x = logic.origin_pos.x + logic.movement_range * cos(t);
        float new_y = logic.origin_pos.y + logic.movement_range * sin(t);

        transform.position.x = new_x;
        transform.position.y = new_y;
    }
}