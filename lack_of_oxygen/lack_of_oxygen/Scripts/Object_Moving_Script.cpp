
#include "../Scripts/Object_Moving_Script.h"

// Include the manager headers
#include "../Manager/ECS_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Utility/Globals.h"
#include "../Utility/Type.h"

namespace lof {

    void Object_Moving_Script::register_script() {
        std::shared_ptr<Script> script = std::make_shared<Object_Moving_Script>();
        LGS.add_script("object_moving_script", script);
    }

    void Object_Moving_Script::init_get_movement_data(Entity* entity) {
        //get the data
        if (!entity || !ECSM.has_component<Logic_Component>(entity->get_id()) || !ECSM.has_component<Transform2D>(entity->get_id())) {
            return;
        }
        auto& logic_comp = ECSM.get_component<Logic_Component>(entity->get_id());

        ScriptData script_data = logic_comp.get_script_data("object_moving_script", "update_function");
        //need to retricve the script data to the respective values in each private? or in component which better?

        try {
            EntityData& data = entity_data[entity->get_id()]; // Create or get existing data for this entity

            data.movement_pattern = logic_comp.get_script_individual_data<std::string>(script_data, "movement_pattern");
            data.movement_speed = logic_comp.get_script_individual_data<float>(script_data, "movement_speed");
            data.movement_range = logic_comp.get_script_individual_data<float>(script_data, "movement_range");
            data.origin_pos = logic_comp.get_script_individual_data<Vec2D>(script_data, "origin_pos");
            data.reverse_direction = logic_comp.get_script_individual_data<bool>(script_data, "reverse_direction");
            data.rotate_with_motion = logic_comp.get_script_individual_data<bool>(script_data, "rotate_with_motion");
            data.timer = 0.0f;

            if (data.movement_pattern == "linear") {
                data.pattern = 1;
            }
            else if (data.movement_pattern == "circular") {
                data.pattern = 2;
            }
            else {
                data.pattern = 0;
            }
        }
        catch (const std::exception& e) {
            LM.write_log("Failed to initialize component: %s", e.what());
            return;
        }
    }

    void Object_Moving_Script::update_obj_movement(Entity* entity) {
        
        //get entity logic component
        //return if retrival fail
        //get entity transform component
        //return if fail
        //get the param for all the values it require
        //and updates the others.

        if (!entity || !ECSM.has_component<Logic_Component>(entity->get_id()) ||
            !ECSM.has_component<Transform2D>(entity->get_id())) {
            return;
        }

        auto it = entity_data.find(entity->get_id());
        if (it == entity_data.end()) {
            return; // No data for this entity
        }

        auto& data = it->second;
        auto& transform_comp = ECSM.get_component<Transform2D>(entity->get_id());

        switch (data.pattern) {
        case 1:
            update_linear_movement(data, transform_comp);
            break;

        case 2:
            update_circular_movement(data, transform_comp);
            break;

        default:
            LM.write_log("WARNING: Unknown movement pattern %d for entity [%d] '%s'",
                data.pattern,
                entity->get_id(),
                entity->get_name().c_str());
            break;
        }

        
    }

    void Object_Moving_Script::update_linear_movement(EntityData& data, Transform2D& transform_comp) {
        //(void)delta_time;
        float t = data.movement_speed * data.timer;

        if (!data.reverse_direction) {
            // Horizontal movement
            float new_x = data.origin_pos.x + data.movement_range * sin(t);
            transform_comp.position.x = new_x;
        }
        else {
            // Vertical movement
            float new_y = data.origin_pos.y + data.movement_range * sin(t);
            transform_comp.position.y = new_y;
        }
    }

    void Object_Moving_Script::update_circular_movement(EntityData& data, Transform2D& transform_comp) {
        float t = data.movement_speed * data.timer;

        // Calculate new position using parametric equations of a circle
        float new_x = data.origin_pos.x + data.movement_range * cos(t);
        float new_y = data.origin_pos.y + data.movement_range * sin(t);

        transform_comp.position.x = new_x;
        transform_comp.position.y = new_y;
    }
}