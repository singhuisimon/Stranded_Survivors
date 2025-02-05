
#include "../Scripts/Object_Moving_Script.h"

#include <memory>
#include <mutex>

// Include the manager headers
#include "../Manager/ECS_Manager.h"
#include "../Manager/Log_Manager.h"
#include "../Manager/Input_Manager.h"
#include "../Utility/Globals.h"
#include "../Utility/Type.h"
#include "../Main/Main.h"

namespace lof {

    Object_Moving_Script::Object_Moving_Script(){
        entity_data = std::unordered_map<EntityID, EntityData>();
    }

    void Object_Moving_Script::register_script() {
        auto script = std::make_shared<Object_Moving_Script>();

        //std::weak_ptr<Object_Moving_Script> weak_this = std::dynamic_pointer_cast<Object_Moving_Script>(script);

        script->add_function("init", [script](EntityID entity_id) {

        if (!entity_id || !ECSM.has_component<Logic_Component>(entity_id) ||
            !ECSM.has_component<Transform2D>(entity_id)) {
            return;
        }

        LM.write_log("Object_Moving_Script hi %u", entity_id);



        auto& logic_comp = ECSM.get_component<Logic_Component>(entity_id);
        ScriptData script_data = logic_comp.get_script_data("object_moving_script", "update");

        try {
            EntityData data;

            data.movement_pattern = logic_comp.get_script_individual_data<int>(script_data, "movement_pattern");
            data.movement_speed = logic_comp.get_script_individual_data<float>(script_data, "movement_speed");
            data.movement_range = logic_comp.get_script_individual_data<float>(script_data, "movement_range");
            data.origin_pos = logic_comp.get_script_individual_data<Vec2D>(script_data, "origin_pos");
            data.reverse_direction = logic_comp.get_script_individual_data<bool>(script_data, "reverse_direction");
            data.rotate_with_motion = logic_comp.get_script_individual_data<bool>(script_data, "rotate_with_motion");
            data.timer = 0.0f;

            if (data.movement_pattern == 0) {
                data.pattern = 1;
            }
            else if (data.movement_pattern == 1) {
                data.pattern = 2;
            }
            else {
                data.pattern = 0;
            }

            /* auto result = entity_data.emplace(*entity_id, data);
             if (!result.second) {
                 LM.write_log("Entity data already exists for entity %u", *entity_id);
             }*/


             //EntityID id = entity_id;
             //entity_data[id] = data;

            script->entity_data[entity_id] = data;

        }
        catch (const std::exception& e) {
            LM.write_log("Failed to initialize component: %s", e.what());
            return;
        }
            
        });

        script->add_function("update", [script](EntityID entity_id) {
            if (!entity_id || !ECSM.has_component<Logic_Component>(entity_id) ||
                !ECSM.has_component<Transform2D>(entity_id)) {
                return;
            }

            auto it = script->entity_data.find(entity_id);
            if (it == script->entity_data.end()) {
                return; // No movement data stored for this entity
            }

            auto& data = it->second;
            auto& transform_comp = ECSM.get_component<Transform2D>(entity_id);

            // Update movement timer
            data.timer += delta_time; // Assuming delta_time is globally available

            // Apply the correct movement pattern
            switch (data.pattern) {
            case 1: // Linear
                script->update_linear_movement(data, transform_comp);
                break;

            case 2: // Circular
                script->update_circular_movement(data, transform_comp);
                break;

            default:
                LM.write_log("WARNING: Unknown movement pattern %d for entity [%d]", data.pattern, entity_id);
                break;
            }

            });

        // End Function (Cleanup)
        script->add_function("end", [script](EntityID entity_id) {
            if (!entity_id) {
                return;
            }

            // Remove the entity's movement data from the static map
            auto it = script->entity_data.find(entity_id);
            if (it != script->entity_data.end()) {
                script->entity_data.erase(it);
            }
            });


        LGS.add_script("object_moving_script", script);
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

    
    // Helper methods implementation
    void Object_Moving_Script::add_entity_data(EntityID id, const EntityData& data) {
        entity_data[id] = data;
    }

    bool Object_Moving_Script::get_entity_data(EntityID id, EntityData& out_data) const {
        //std::lock_guard<std::mutex> lock(entity_data_mutex);
        auto it = entity_data.find(id);
        if (it != entity_data.end()) {
            out_data = it->second;
            return true;
        }
        return false;
    }

    void Object_Moving_Script::remove_entity_data(EntityID id) {
        //std::lock_guard<std::mutex> lock(entity_data_mutex);
        entity_data.erase(id);
    }
}