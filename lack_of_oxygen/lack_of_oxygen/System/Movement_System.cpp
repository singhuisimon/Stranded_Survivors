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
//#include "../Utility/Force_Manager.h"
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

    void Movement_System::integrate(float delta_time) {

#if 0
        // Iterate through entities matching the system's signature
        for (EntityID entity_id : get_entities()) {

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
            auto& physics = ECSM.get_component<Physics_Component>(entity_id);

            // Skip processing for static entities
            if (physics.get_is_static())
                continue;

            // Store the current position before updating
            transform.prev_position = transform.position;


            //// Handle jumping mechanics
            //if (IM.is_key_held(GLFW_KEY_SPACE) && physics.get_is_grounded()) { //&& physics.get_is_grounded() && !physics.get_is_jumping()) {
            //  
            //    physics.apply_force(Vec2D(0.0f, physics.get_jump_force() * physics.get_mass()));
            //    physics.set_is_grounded(false);
            //    physics.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
            //}

            // Apply gravity
            Vec2D gravity_force = physics.get_gravity() * physics.get_mass() * GRAVITY_ACCELERATOR;
            physics.apply_force(gravity_force);

            // Handle horizontal movement
            if (IM.is_key_held(GLFW_KEY_A)) {
                // Apply force to move left
                //physics.apply_force(Vec2D(-200.f * delta_time, 0.0f));
                velocity.velocity.x = -DEFAULT_SPEED;
            }
            else if (IM.is_key_held(GLFW_KEY_D)) {
                // Apply force to move right
                //physics.apply_force(Vec2D(200.f * delta_time, 0.0f));
                velocity.velocity.x = DEFAULT_SPEED ;
            }

            // Calculate the acceleration
            Vec2D acceleration = physics.get_accumulated_force() * physics.get_inv_mass();

            // Update velocity according to the acceleration
            velocity.velocity += acceleration * delta_time;

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
            std::cout << "Accumulated Force in movement system before reset: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << "\n";
            std::cout << "Force Applied: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << "\n";
            std::cout << "Acceleration: " << acceleration.x << ", " << acceleration.y << "\n";
            std::cout << "New Velocity: " << velocity.velocity.x << ", " << velocity.velocity.y << "\n";
            std::cout << "Position: " << transform.position.x << ", " << transform.position.y << "\n";
            std::cout << "Prev Position: " << transform.prev_position.x << ", " << transform.prev_position.y << "\n\n";
            std::cout << "State of player on ground: " << physics.get_is_grounded() << "\n \n";




            // Reset the accumulated force
            physics.reset_forces();
            std::cout << "Accumulated Forces after reset: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << '\n';
    }
#endif 


#if 1
        // Iterate through entities matching the system's signature

        std::cout << "Movement system is now update\n\n";
        //LM.write_log("Movement system is update now");

        for (EntityID entity_id : get_entities()) {
            // std::cout << entity_id << "in physic \n\n";

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
            auto& physics = ECSM.get_component<Physics_Component>(entity_id);
            auto& collision = ECSM.get_component<Collision_Component>(entity_id);

            // Skip processing for static entities
            if (physics.get_is_static())
                continue;

            // Store the current position before updating
            transform.prev_position = transform.position;
 
            // Handle jumping mechanics
            if (IM.is_key_held(GLFW_KEY_SPACE)) { //&& physics.get_is_grounded()) {
                // Apply jump force
                std::cout << " this is jump\n";
                std::cout << "Jump applied with force: " << physics.get_jump_force() * physics.get_mass() << 
                    "\n\n *********************************************";
                physics.apply_force(Vec2D(0.0f, physics.get_jump_force() * physics.get_mass()));
                physics.set_is_grounded(false);
                physics.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));

            }


            // Handle horizontal movement
            if (IM.is_key_held(GLFW_KEY_A)) {
                // Apply force to move left
                std::cout << "move left pls\n";
                velocity.velocity.x = -DEFAULT_SPEED;
            }
            else if (IM.is_key_held(GLFW_KEY_D)) {
                // Apply force to move right
                std::cout << "move right pls\n";
                velocity.velocity.x = DEFAULT_SPEED;
            }

            physics.apply_force(physics.get_gravity() * physics.get_mass());
            // Calculate the acceleration

            Vec2D resulting_acc = physics.get_acceleration();
            resulting_acc = physics.get_accumulated_force() * physics.get_inv_mass() + physics.get_gravity();
            
            physics.set_acceleration(resulting_acc);

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
            std::cout << "Accumulated Force in movement system before reset: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << "\n";
            std::cout << "Force Applied: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << "\n";
            std::cout << "Acceleration: " << physics.get_acceleration().x << ", " << physics.get_acceleration().y << "\n";
            std::cout << "New Velocity: " << velocity.velocity.x << ", " << velocity.velocity.y << "\n";
            std::cout << "Position: " << transform.position.x << ", " << transform.position.y << "\n";
            std::cout << "Prev Position: " << transform.prev_position.x << ", " << transform.prev_position.y << "\n\n";
            std::cout << "State of player on ground: " << physics.get_is_grounded() << "\n \n";

            // Reset the accumulated force
            physics.reset_forces();
            std::cout << "Accumulated Forces after reset: " << physics.get_accumulated_force().x << ", " << physics.get_accumulated_force().y << '\n';

            // Do not reset is_grounded or is_jumping here
            // Let the collision system update is_grounded
        }


#endif
    }

#if 0
    void Movement_System::resolve_physics(const std::vector<CollisionPair>& collisions, float delta_time) {
        for (const auto& collision : collisions) {
            apply_physics_response(collision.entity1, collision, delta_time);

            auto& physics2 = ECSM.get_component<Physics_Component>(collision.entity2);
            if (!physics2.get_is_static()) {
                apply_physics_response(collision.entity2, collision, delta_time);
            }
        }
    }

    void Movement_System::apply_physics_response(EntityID entity_id, const CollisionPair& collision, float delta_time) {
        auto& physics = ECSM.get_component<Physics_Component>(entity_id);
        auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
        auto& transform = ECSM.get_component<Transform2D>(entity_id);

        if (physics.get_is_static()) {
            return;
        }

        // Handle different collision cases
        switch (collision.side) {
        case CollisionSide::BOTTOM:
            handle_ground_contact(entity_id, collision, delta_time);
            break;

        case CollisionSide::TOP:
            // Ceiling collision
            if (velocity.velocity.y > 0) {
                velocity.velocity.y = -velocity.velocity.y * 0.1f; // Slight bounce
            }
            transform.position.y -= collision.overlap.y;
            break;

        case CollisionSide::LEFT:
        case CollisionSide::RIGHT:
            handle_wall_contact(entity_id, collision, delta_time);
            break;

        default:
            break;
        }
    }


    void Movement_System::handle_ground_contact(EntityID entity_id, const CollisionPair& collision, float delta_time) {
        auto& physics = ECSM.get_component<Physics_Component>(entity_id);
        auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
        auto& transform = ECSM.get_component<Transform2D>(entity_id);

        // Set grounded state
        physics.set_is_grounded(true);
        physics.set_gravity(Vec2D(0.0f, 0.0f));

        // Resolve position
        transform.position.y += collision.overlap.y;

        // Apply normal force to counter gravity
        Vec2D normal_force = Vec2D(0.0f, -physics.get_gravity().y * physics.get_mass());
        physics.apply_force(normal_force);

        // Handle vertical velocity
        if (velocity.velocity.y < 0) {
            float restitution = 0.1f; // Slight bounce
            velocity.velocity.y = -velocity.velocity.y * restitution;

            // If bounce is very small, just stop
            if (std::abs(velocity.velocity.y) < 0.1f) {
                velocity.velocity.y = 0.0f;
            }
        }

        // Apply ground friction to horizontal movement
        float friction_coefficient = 0.3f;
        velocity.velocity.x *= (1.0f - friction_coefficient);

        //LM.write_log("Ground contact handled for entity " + std::to_string(entity_id));
    }

    void Movement_System::handle_wall_contact(EntityID entity_id, const CollisionPair& collision, float delta_time) {
        auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
        auto& transform = ECSM.get_component<Transform2D>(entity_id);

        // Resolve position
        if (collision.side == CollisionSide::LEFT) {
            transform.position.x += collision.overlap.x;
        }
        else {
            transform.position.x -= collision.overlap.x;
        }

        // Handle horizontal velocity
        float wall_restitution = 0.2f; // Wall bounce factor
        velocity.velocity.x = -velocity.velocity.x * wall_restitution;

        // If velocity is very small, stop horizontal movement
        if (std::abs(velocity.velocity.x) < 0.1f) {
            velocity.velocity.x = 0.0f;
        }

        //LM.write_log("Wall contact handled for entity " + std::to_string(entity_id));
    }

#endif
    void Movement_System::update(float delta_time) {

        Movement_System::integrate(delta_time);
        
       // Movement_System::apply_physics_response(delta_time);

       
    }

    std::string Movement_System::get_type() const {
        return "Movement_System";
    }

} // namespace lof
