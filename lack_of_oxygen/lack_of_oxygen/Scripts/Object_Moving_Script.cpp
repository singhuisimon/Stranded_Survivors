
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
        entity_data = std::unordered_map<EntityID, MovementData>();
    }

    Object_Moving_Script::~Object_Moving_Script() {
		cleanup();
		LM.write_log("Object_Moving_Script::clean up complete");
    }

	void Object_Moving_Script::cleanup() {
        LM.write_log("Object_Moving_Script::cleanup() called");
		entity_data.clear();
	}

    void Object_Moving_Script::register_script() {
        std::shared_ptr<Object_Moving_Script> object_moving_script = std::make_shared<Object_Moving_Script>();

        std::weak_ptr<Object_Moving_Script> weak_script = object_moving_script;

        object_moving_script->add_function("init", [weak_script](EntityID entity_id) {
		    auto object_moving_script = weak_script.lock();
            if (!entity_id || !ECSM.has_component<Logic_Component>(entity_id) ||
                !ECSM.has_component<Transform2D>(entity_id)) {
		    	LM.write_log("Object_Moving_Script::register_script(): Entity %d does not have required components.", entity_id);
                return;
            }

            auto& logic_comp = ECSM.get_component<Logic_Component>(entity_id);
            ScriptData script_data = logic_comp.get_script_data("object_moving_script", "update");

            try {
                MovementData data;

                data.movement_pattern = logic_comp.get_script_individual_data<int>(script_data, "movement_pattern");
                data.movement_speed = logic_comp.get_script_individual_data<float>(script_data, "movement_speed");
                data.movement_range = logic_comp.get_script_individual_data<float>(script_data, "movement_range");
                data.origin_pos = logic_comp.get_script_individual_data<Vec2D>(script_data, "origin_pos");
                data.reverse_direction = logic_comp.get_script_individual_data<bool>(script_data, "reverse_direction");
                data.rotate_with_motion = logic_comp.get_script_individual_data<bool>(script_data, "rotate_with_motion");
                data.timer = 0.0f;

                if (data.movement_pattern == 0) {
		    		// Linear movement
                    data.pattern = 1;
                }
                else if (data.movement_pattern == 1) {
		    		// Circular movement
                    data.pattern = 2;
                }
                else {
                    //unknown;
                    data.pattern = 0;
                }

                object_moving_script->entity_data[entity_id] = data;

            }
            catch (const std::exception& e) {
                LM.write_log("Failed to initialize component: %s", e.what());
                return;
            }
            
        });

        object_moving_script->add_function("update", [weak_script](EntityID entity_id) {
			auto object_moving_script = weak_script.lock();
            if (!entity_id || !ECSM.has_component<Logic_Component>(entity_id) ||
                !ECSM.has_component<Transform2D>(entity_id)) {
                return;
            }

            auto it = object_moving_script->entity_data.find(entity_id);
            if (it == object_moving_script->entity_data.end()) {
                return; // No movement data stored for this entity
            }

            auto& data = it->second;
            auto& transform_comp = ECSM.get_component<Transform2D>(entity_id);

            // Update movement timer
            float delta_time = FPSM.get_delta_time();
            data.timer += delta_time; // Assuming delta_time is globally available

            // Apply the correct movement pattern
            switch (data.pattern) {
            case 1: // Linear
                object_moving_script->update_linear_movement(data, transform_comp);
                break;

            case 2: // Circular
                object_moving_script->update_circular_movement(data, transform_comp);
                break;

            default:
                LM.write_log("WARNING: Unknown movement pattern %d for entity [%d]", data.pattern, entity_id);
                break;
            }

            });

        // End Function (Cleanup)
        object_moving_script->add_function("end", [weak_script](EntityID entity_id) {
			auto object_moving_script = weak_script.lock();
            if (!entity_id) {
                return;
            }

            // Remove the entity's movement data from the static map
            auto it = object_moving_script->entity_data.find(entity_id);
            if (it != object_moving_script->entity_data.end()) {
                object_moving_script->entity_data.erase(it);
            }
            });


        LGS.add_script("object_moving_script", object_moving_script);
    }

    void Object_Moving_Script::update_linear_movement(MovementData& data, Transform2D& transform_comp) {

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

    void Object_Moving_Script::update_circular_movement(MovementData& data, Transform2D& transform_comp) {
        float t = data.movement_speed * data.timer;

        // Calculate new position using parametric equations of a circle
        float new_x = data.origin_pos.x + data.movement_range * cos(t);
        float new_y = data.origin_pos.y + data.movement_range * sin(t);

        transform_comp.position.x = new_x;
        transform_comp.position.y = new_y;
    }
    
    // Helper methods implementation
    void Object_Moving_Script::add_entity_data(EntityID id, const MovementData& data) {
        entity_data[id] = data;
    }

    bool Object_Moving_Script::get_entity_data(EntityID id, MovementData& out_data) const {
        auto it = entity_data.find(id);
        if (it != entity_data.end()) {
            out_data = it->second;
            return true;
        }
        return false;
    }

    void Object_Moving_Script::remove_entity_data(EntityID id) {
        entity_data.erase(id);
    }
}