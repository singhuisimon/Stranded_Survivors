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


    //Integration 
    //collision detection
    //collision resolution
    //update according to FPS? **need to see
    void Movement_System::update(float delta_time) {
        // Iterate through all entities and update positions based on velocity
        const auto& entities = ecs.get_entities();
        for (const auto& entity : entities) {
            EntityID id = entity->get_id();

            Mass_Component* mass = nullptr;

            //skip if the entity does not have mass
            if (entity->has_component(ecs.get_component_id<Mass_Component>())) {
               



                mass = &ecs.get_component<Mass_Component>(id);

                std::cout << "Mass of obj: " << mass << std::endl;

                //skipi if the entity is static
                if (mass->is_static) continue;
            }

            // Check if entity has both Transform2D and Velocity_Component
            if (entity->has_component(ecs.get_component_id<Transform2D>()) &&
                entity->has_component(ecs.get_component_id<Velocity_Component>()) &&
                entity->has_component(ecs.get_component_id<Physics_Component>())) {

                Transform2D& transform = ecs.get_component<Transform2D>(id);
                Velocity_Component& velocity = ecs.get_component<Velocity_Component>(id);
                Physics_Component& physics = ecs.get_component<Physics_Component>(id);


                if (mass && !mass->is_static) {

                    //calculate the acceleration 

                    Vec2D total_force = physics.accumulated_force + (physics.gravity * mass->mass); // F = ma = mg; g as acceleration: a(g) = F / m;
                    Vec2D acceleration = total_force * mass->inv_mass; // F = ma; a = F / m;

                    //update velocity according to the acceleration on each body
                    velocity.velocity += acceleration * delta_time;

                }
                //dampen velocity 
                velocity.velocity *= physics.damping_factor; 

                // Update position based on velocity and delta_time
                transform.position += velocity.velocity * delta_time;


                //clamp velocity to max velocity 
                float squared_velocity = square_length_vec2d(velocity.velocity);

                if (squared_velocity > physics.max_velocity * physics.max_velocity) {
                    Vec2D normalize_result; 
                    normalize_vec2d(normalize_result, velocity.velocity);
                    velocity.velocity = normalize_result * physics.max_velocity;
                }

                //reset the accumulated force 
                physics.accumulated_force = Vec2D(0, 0);
            }
        }

    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof
