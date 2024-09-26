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

                Mass_Component* mass;

                //check if the entity has mass 
                if (entity->has_component(ecs.get_component_id<Mass_Component>())) {
                    mass = &ecs.get_component<Mass_Component>(id);

                    std::cout << "Mass of obj: " << mass << std::endl;
                }
                else mass = nullptr;


                if (mass && !mass->is_static) {

                    //Update velocity
                    Vec2D gravity(0, -9.8f);

                    Vec2D acceleration = gravity * mass->inv_mass; // F = ma; a = F / m; 
                    velocity.velocity.x += acceleration.x * delta_time;
                    velocity.velocity.y += acceleration.y * delta_time;

                }
                //dampen velocity 
                float damping_factor = 0.98f; //0.9f in the demo 
                velocity.velocity.x *= damping_factor; 
                velocity.velocity.y *= damping_factor;

                // Update position based on velocity and delta_time
                transform.position.x += velocity.velocity.x * delta_time;
                transform.position.y += velocity.velocity.y * delta_time;


                //clamp velocity to max velocity 
                const float max_velocity = 1000.0f; 
                float squared_velocity = square_length_vec2d(velocity.velocity);

                if (squared_velocity > max_velocity * max_velocity) {
                    Vec2D normalize_result; 
                    normalize_vec2d(normalize_result, velocity.velocity);
                    velocity.velocity = normalize_result * max_velocity;
                }



            }

        
        }
    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof
