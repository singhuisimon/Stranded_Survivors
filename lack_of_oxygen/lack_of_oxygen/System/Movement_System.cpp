/**
 * @file Movement_System.cpp
 * @brief Implements the Movement_System class.
 * @date September 15, 2024
 */

#include "Movement_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"


namespace lof {

    Movement_System::Movement_System(ECS_Manager& ecs_manager) 
        : ecs(ecs_manager) {}

    void Movement_System::update(float delta_time) {
        // Iterate through all entities and update positions based on velocity
        const auto& entities = ecs.get_entities();
        for (const auto& entity : entities) {
            EntityID id = entity->get_id();


            // Check if entity has both Transform2D, Velocity_Component
            if (entity->has_component(ecs.get_component_id<Transform2D>()) &&
                entity->has_component(ecs.get_component_id<Velocity_Component>()) &&
                entity->has_component(ecs.get_component_id<Physics_Component>()))
                {

                Transform2D& transform = ecs.get_component<Transform2D>(id);
                Velocity_Component& velocity = ecs.get_component<Velocity_Component>(id);
                Physics_Component& physics = ecs.get_component<Physics_Component>(id);

                if (physics.is_static) continue;

                //apply speed 
                float speed = 500.f; 

        
                 // Update position based on velocity and delta_time and input
                if (IM.is_key_held(GLFW_KEY_W)) {

                    velocity.velocity.y = speed; 

                }
                else if (IM.is_key_held(GLFW_KEY_S)) {

                    velocity.velocity.y = -speed;

                }
                else {
                    velocity.velocity.y = 0; 
                }


                if (IM.is_key_held(GLFW_KEY_A)) {

                    velocity.velocity.x = -speed;

                }
                else if (IM.is_key_held(GLFW_KEY_D)) {

                    velocity.velocity.x = speed;

                }
                else {
                    velocity.velocity.x = 0;
                }

                //calculate the acceleration 

                Vec2D total_force = physics.accumulated_force + (physics.gravity * physics.mass); // F = ma = mg; g as acceleration: a(g) = F / m;
                Vec2D acceleration = total_force * physics.inv_mass; // F = ma; a = F / m;

                //update velocity according to the acceleration on each entity
                velocity.velocity += acceleration * delta_time;


                //dampen velocity
                velocity.velocity *= physics.damping_factor;


                transform.position += velocity.velocity * delta_time;


                //clamp velocity to max velocity 
                float squared_velocity = square_length_vec2d(velocity.velocity);

                //squared max_velocity 
                float squared_max_vel = physics.max_velocity * physics.max_velocity; 


                // length of entity's velocity > max_velocity
                if (squared_velocity > squared_max_vel) {


                        Vec2D normalize_result;
                        normalize_vec2d(normalize_result, velocity.velocity);
                        velocity.velocity = normalize_result * physics.max_velocity;


                }

                //reset the accumulated force 
                physics.reset_forces();

            }
        }
    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof
