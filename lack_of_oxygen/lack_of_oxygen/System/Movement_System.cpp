/**
 * @file Movement_System.cpp
 * @brief Implements the Movement_System class.
 * @date September 15, 2024
 */

#include "Movement_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"

namespace lof {

    Movement_System::Movement_System(ECS_Manager& ecs_manager)
        : ecs(ecs_manager) {}

    void Movement_System::update(float delta_time) {
        // Iterate through all entities and update positions based on velocity
        const auto& entities = ecs.get_entities();
        for (const auto& entity : entities) {
            EntityID id = entity->get_id();

            // Check if entity has both Transform2D and Velocity_Component
            if (entity->has_component(ecs.get_component_id<Transform2D>()) &&
                entity->has_component(ecs.get_component_id<Velocity_Component>())) {

                Transform2D& transform = ecs.get_component<Transform2D>(id);
                Velocity_Component& velocity = ecs.get_component<Velocity_Component>(id);

                // Update position based on velocity and delta_time
                transform.position.x += velocity.velocity.x * delta_time;
                transform.position.y += velocity.velocity.y * delta_time;
            }
        }
    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof
