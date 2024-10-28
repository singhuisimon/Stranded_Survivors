/**
 * @file Movement_System.cpp
 * @brief Implements the physics (gravity, jumping) and movement for the game entities
 * @author Wai Lwin Thit (100%)
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#include "Movement_System.h"
#include "../Utility/Constant.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"

namespace lof {

    Movement_System::Movement_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Transform2D>());
        signature.set(ECSM.get_component_id<Velocity_Component>());
        signature.set(ECSM.get_component_id<Physics_Component>());
    }

    void Movement_System::update(float delta_time) {
        // Iterate through entities matching the system's signature
        for (EntityID entity_id : get_entities()) {

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
            auto& physics = ECSM.get_component<Physics_Component>(entity_id);

            // Skip processing for static entities
            if (physics.is_static)
                continue;

            // Handle jumping mechanics
            if (IM.is_key_held(GLFW_KEY_SPACE) && physics.is_grounded && !physics.is_jumping) {
                // Apply jump force
                physics.apply_force(Vec2D(0.0f, physics.jump_force * physics.mass));

                physics.is_jumping = true;   // Mark as jumping
                physics.is_grounded = false; // Leave the ground
            }

            // Apply gravity
            physics.apply_force(physics.gravity * physics.mass);

            // Handle horizontal movement
            if (IM.is_key_held(GLFW_KEY_A)) {
                // Apply force to move left
                physics.apply_force(Vec2D(-10.f, 0.0f));
            }
            else if (IM.is_key_held(GLFW_KEY_D)) {
                // Apply force to move right
                physics.apply_force(Vec2D(10.f, 0.0f));
            }

            // Calculate the acceleration
            Vec2D acceleration = physics.accumulated_force * physics.inv_mass;

            // Update velocity according to the acceleration
            velocity.velocity += acceleration * delta_time;

            // Dampen velocity
            velocity.velocity *= physics.damping_factor;

            // Update the position based on velocity
            transform.position += velocity.velocity * delta_time;

            // Clamp velocity to max velocity
            float squared_velocity = square_length_vec2d(velocity.velocity);
            float squared_max_vel = physics.max_velocity * physics.max_velocity;

            // If length of entity's velocity > max_velocity
            if (squared_velocity > squared_max_vel) {
                Vec2D normalize_result;
                normalize_vec2d(normalize_result, velocity.velocity);
                velocity.velocity = normalize_result * physics.max_velocity;
            }

            // Reset the accumulated force
            physics.reset_forces();

            // Do not reset is_grounded or is_jumping here
            // Let the collision system update is_grounded
        }
    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof