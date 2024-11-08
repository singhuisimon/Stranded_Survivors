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
#include "../Utility/Force_Helper.h"
#include "../Utility/Constant.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"
#include "../System/Render_System.h"
#include "Collision_System.h"

namespace lof {

     /**
     * @brief Constructor for Movement_System.
     * Initializes the system's signature.
     */


    Movement_System::Movement_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Transform2D>());
        signature.set(ECSM.get_component_id<Velocity_Component>());
        signature.set(ECSM.get_component_id<Physics_Component>());
    }
    /**
     * @brief Integrates physics calculations for movement, applying forces and updating positions.
     * @param delta_time The time increment for updating entity positions and velocities.
     */
    void Movement_System::integrate(float delta_time) {

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
            if (physics.get_jump_requested() && physics.get_is_grounded() && !physics.get_has_jumped()) {

                                
                physics.force_helper.activate_force(JUMP_UP);
                physics.set_is_grounded(false);
                physics.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                physics.set_has_jumped(true);

                velocity.velocity.y = physics.get_jump_force();
                //reset the jump request
                physics.reset_jump_request();
                physics.force_helper.deactivate_force(JUMP_UP);  // Deactivate the jump force

                ECSM.get_component<Audio_Component>(entity_id).set_audio_state("jumping", PLAYING);
            }

            //update forces based on time
            physics.force_helper.update_force(delta_time);

            //get resultant force 
            Vec2D sum_force = physics.force_helper.get_resultant_Force();

            //add gravity 
            sum_force += physics.get_gravity() * physics.get_mass();


            //save the accumulated forces
            physics.apply_force(sum_force);

            // Calculate the acceleration
            Vec2D resulting_acceleration = physics.get_accumulated_force() * physics.get_inv_mass();


            physics.set_acceleration(resulting_acceleration);


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

            // Reset the accumulated force
            physics.reset_forces();
        }

    }

    /**
     * @brief Updates the system.
     * @param delta_time The time elapsed since the last update.
     */
    void Movement_System::update(float delta_time) {

        Movement_System::integrate(delta_time);

    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof


#endif 

