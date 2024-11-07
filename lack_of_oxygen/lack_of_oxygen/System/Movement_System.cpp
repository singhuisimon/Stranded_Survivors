/**
 * @file Movement_System.cpp
 * @brief Implements the physics (gravity, jumping) and movement for the game entities
 * @author Wai Lwin Thit (100%)
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#if 1
#include "Movement_System.h"
//#include "../Utility/Force_Manager.h"
#include "../Utility/Constant.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"
#include "../System/Render_System.h"
#include "Collision_System.h"

namespace lof {

    

    Movement_System::Movement_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Transform2D>());
        signature.set(ECSM.get_component_id<Velocity_Component>());
        signature.set(ECSM.get_component_id<Physics_Component>());
    }

    void Movement_System::integrate(float delta_time) {
#if 1
        // Iterate through entities matching the system's signature

        LM.write_log("Movement system start update");

        for (EntityID entity_id : get_entities()) {
            // std::cout << entity_id << "in physic \n\n";

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
            auto& physics = ECSM.get_component<Physics_Component>(entity_id);

            // Skip processing for static entities
            if (physics.get_is_static())
                continue;

            // Store the current position before updating
            transform.prev_position = transform.position;
 
            // Handle jumping mechanics
            if (IM.is_key_held(GLFW_KEY_SPACE) && physics.get_is_grounded() && !physics.get_has_jumped()) {
              
                std::cout << "DEBUG: player has pressed space\n";
                physics.force_manager.activate_force(JUMP_UP);
                physics.set_is_grounded(false);
                physics.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                physics.set_has_jumped(true);
               
                velocity.velocity.y = physics.get_jump_force();
            }
        
            else if (!IM.is_key_held(GLFW_KEY_SPACE)) {
                std::cout << "DEBUG: player has NOT pressed space\n";
                physics.force_manager.deactivate_force(JUMP_UP);  // Deactivate the jump force
            }


            // Handle horizontal movement
            if (IM.is_key_held(GLFW_KEY_A)) {
                // Apply force to move left
                physics.force_manager.activate_force(MOVE_LEFT);
            }
            else {
                physics.force_manager.deactivate_force(MOVE_LEFT);
            }

            if (IM.is_key_held(GLFW_KEY_D)) {
                // Apply force to move right
                physics.force_manager.activate_force(MOVE_RIGHT);

            }
            else {
                physics.force_manager.deactivate_force(MOVE_RIGHT);
            }


            //update forces based on time
            physics.force_manager.update_force(delta_time);

            //get resultant force 
            Vec2D sum_force = physics.force_manager.get_resultant_Force();

            //add gravity 
                sum_force += physics.get_gravity() * physics.get_mass();

            std::cout << "Force_Manager: Sum_Forces: " << sum_force.x << ", " << sum_force.y << std::endl;

            //save the accumulated forces
            physics.apply_force(sum_force);

            // Calculate the acceleration
            Vec2D resulting_acceleration = physics.get_accumulated_force() * physics.get_inv_mass();

            std::cout << "Inverse mass: " << physics.get_inv_mass();

            physics.set_acceleration(resulting_acceleration);

            std::cout << "Acceleration: " << physics.get_acceleration().x << ", " << physics.get_acceleration().y << '\n';

            // Update velocity according to the acceleration
            velocity.velocity += physics.get_acceleration() * delta_time;

            // Dampen velocity
            velocity.velocity *= physics.get_damping_factor();

            // Update the position based on velocity
            transform.position += velocity.velocity * delta_time;

            // Clamp velocity to max velocity
            float squared_velocity = square_length_vec2d(velocity.velocity);

            // If length of entity's velocity > max_velocity
            if (squared_velocity > physics.get_max_velocity_sq()) {
                Vec2D normalize_result;
                normalize_vec2d(normalize_result, velocity.velocity);
                velocity.velocity = normalize_result * physics.get_max_velocity();

            }

            std::cout << "Physic System side: \n";
            std::cout << "Entity ID " << entity_id << "\n";
#if 0
            std::cout << "Accumulated Force in movement system before reset: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << "\n";
            std::cout << "Force Applied: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << "\n";
            std::cout << "Acceleration: " << physics.get_acceleration().x << ", " << physics.get_acceleration().y << "\n";
            std::cout << "New Velocity: " << velocity.velocity.x << ", " << velocity.velocity.y << "\n";
            std::cout << "Position: " << transform.position.x << ", " << transform.position.y << "\n";
            std::cout << "Prev Position: " << transform.prev_position.x << ", " << transform.prev_position.y << "\n\n";
#endif
            std::cout << "State of player on ground: " << physics.get_is_grounded() << "\n";
            std::cout << "State of player jump state: " << physics.get_has_jumped() << "\n";

            // Reset the accumulated force
           physics.reset_forces();
            std::cout << "Accumulated Forces after reset: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << '\n';

            // Do not reset is_grounded or is_jumping here
            // Let the collision system update is_grounded
        }


        LM.write_log("Movement system end update");


#endif
    }
    void Movement_System::update(float delta_time) {

        std::cout << "---------------------------------------START Movement_System-------------------------------------\n";

        Movement_System::integrate(delta_time);

        std::cout << "---------------------------------------END Movement_System-------------------------------------\n";

    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof


#endif 

