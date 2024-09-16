/**
 * @file Movement_System.cpp
 * @brief Implements the Movement_System class for the ECS.
 * @author Simon Chan
 * @date September 15, 2024
 */

// Include header file
#include "Movement_System.h"

// Include file dependencies
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"

namespace lof {

    Movement_System::Movement_System(ECS_Manager& manager) : ecs_manager(manager) {}

    void Movement_System::update(float delta_time) {
        // Iterate over all entities
        for (const auto& entity_ptr : ecs_manager.get_entities()) {
            EntityID entity_id = entity_ptr->get_id();

            // Check if the entity has both Position and Velocity components
            if (entity_ptr->has_component(ecs_manager.get_component_id<Position_Component>()) &&
                entity_ptr->has_component(ecs_manager.get_component_id<Velocity_Component>())) {

                auto& position = ecs_manager.get_component<Position_Component>(entity_id);
                auto& velocity = ecs_manager.get_component<Velocity_Component>(entity_id);

                // Update position based on velocity
                position.x += velocity.vx * delta_time;
                position.y += velocity.vy * delta_time;
            }
        }
    }

} // namespace lof
