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

#if 0
        // Iterate through entities matching the system's signature
        
       // std::cout << "Movement system is now update\n\n";
        LM.write_log("Movement system is update now");
       
        for (EntityID entity_id : get_entities()) {
            

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& velocity = ECSM.get_component<Velocity_Component>(entity_id);
            auto& physics = ECSM.get_component<Physics_Component>(entity_id);
            auto& collision = ECSM.get_component<Collision_Component>(entity_id);

            // Skip processing for static entities
            if (physics.get_is_static())
                continue;
            //std::cout << entity_id << "in physic \n\n";

            // Reset grounded status to allow collision system to set it again
            //physics.is_grounded = false;
            //physics.is_grounded = false;

            // Store the current position before updating
            transform.prev_position = transform.position;

            if (entity_id == 3)
            {
                physics.gravity.y = 0.0f;
                velocity.velocity.x += 50;
            }

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

            // Handle jumping mechanics
            if (IM.is_key_held(GLFW_KEY_SPACE) && physics.is_grounded) {
                // Apply jump force
             //   std::cout << " this is jump\n";
               // std::cout << "Jump applied with force: " << physics.jump_force * physics.mass << "\n *********************************************";
                physics.apply_force(Vec2D(0.0f, physics.jump_force * physics.mass));
                physics.is_grounded = false;
                physics.gravity.y = DEFAULT_GRAVITY;
                ECSM.get_component<Audio_Component>(entity_id).set_audio_state("jumping", PLAYING);  //jumping sound
            }

           
            // Handle horizontal movement
            if (IM.is_key_held(GLFW_KEY_A)&&entity_id == 4) {
                // Apply force to move left
                //physics.apply_force(Vec2D(-200.f * delta_time, 0.0f));
                velocity.velocity.x = -DEFAULT_SPEED;
                std::cout << "move left pls\n";
                physics.apply_force(Vec2D(-5000.f, 0.0f));
                if (physics.is_grounded) {
                    ECSM.get_component<Audio_Component>(entity_id).set_audio_state("moving left", PLAYING); //movement sound
                }
            }
            else if (IM.is_key_held(GLFW_KEY_D) && entity_id == 4) {
                // Apply force to move right
                //physics.apply_force(Vec2D(200.f * delta_time, 0.0f));
                velocity.velocity.x = DEFAULT_SPEED ;
                std::cout << "move right pls\n";
                physics.apply_force(Vec2D(5000.f, 0.0f));
                if (physics.is_grounded) {
                    ECSM.get_component<Audio_Component>(entity_id).set_audio_state("moving right", PLAYING); //movement sound
                }
            }

            if (!physics.is_grounded) {
                physics.apply_force(physics.gravity * physics.mass);
            }
            //physics.apply_force(physics.gravity * physics.mass);
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
#if 0
    void Movement_System::resolve_collision(const CollisionPair& collision, Transform2D& transform, Velocity_Component& velocity, Physics_Component& physics) {

        EntityID entityA = collision.entity1;
        EntityID entityB = collision.entity2;

        // Get the physics and transform components for both entities involved in the collision
        auto& physicsA = ECSM.get_component<Physics_Component>(entityA);
        auto& transformA = ECSM.get_component<Transform2D>(entityA);
        auto& velocityA = ECSM.get_component<Velocity_Component>(entityA);

        auto& physicsB = ECSM.get_component<Physics_Component>(entityB);
        auto& transformB = ECSM.get_component<Transform2D>(entityB);
        auto& velocityB = ECSM.get_component<Velocity_Component>(entityB);

        // Only stop the player (entityA) if it is a dynamic object colliding with a static object (entityB)
        if (!physicsA.is_static && physicsB.is_static) {
            CollisionSide collisionSide = collision.side;

            // Adjust position and velocity based on the collision side
            switch (collisionSide) {
            case CollisionSide::LEFT:
                velocityA.velocity.x = 0.0f;
                transformA.position.x += collision.overlap.x; // Push entity away
                std::cout << "Collided on the LEFT. New position: " << transformA.position.x << "\n";
                break;

            case CollisionSide::RIGHT:
                velocityA.velocity.x = 0.0f;
                transformA.position.x -= collision.overlap.x; // Push entity away
                std::cout << "Collided on the RIGHT. New position: " << transformA.position.x << "\n";
                break;

            case CollisionSide::TOP:
                velocityA.velocity.y = 0.0f;
                transformA.position.y += collision.overlap.y; // Push entity down
                std::cout << "Collided on the TOP. New position: " << transformA.position.y << "\n";
                break;

            case CollisionSide::BOTTOM:
                velocityA.velocity.y = 0.0f;
                transformA.position.y = transformB.position.y + transformB.scale.y * 0.5f + transformA.scale.y * 0.5f; // Ground contact
                std::cout << "Collided on the BOTTOM. New position: " << transformA.position.y << "\n";
                break;

            case CollisionSide::NONE:
                // No collision side to resolve
                break;

            default:
                break;
            }
        }
    }
    

#endif

} // namespace lof

#endif 

#if 0
#include "Movement_System.h"
#include "../Utility/Constant.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Input_Manager.h"

namespace lof
{

    //// Access the singleton Collision_System
    //Collision_System& collisionSystem = Collision_System::get_instance();

    //// Check collisions for this instance
    //int collisionFlags = collisionSystem.Check_Instance_Collision(transform.position.x, transform.position.y, collision.width, collision.height);
    //

    //// Debug output to confirm the call
    //std::cout << "Check_Instance_Collision called, Flags: " << collisionFlags << std::endl;
    //// Handle collision flags
    //if (collisionFlags) {
    //    std::cout << "collide flag is happened now\n";
    //    // Handle collision based on what sides collided
    //    if (collisionFlags & COLLISION_LEFT) {
    //        std::cout << "Collision on the left side!" << std::endl;
    //    }
    //    if (collisionFlags & COLLISION_RIGHT) {
    //        std::cout << "Collision on the right side!" << std::endl;
    //    }
    //    if (collisionFlags & COLLISION_TOP) {
    //        std::cout << "Collision on the top side!" << std::endl;
    //    }
    //    if (collisionFlags & COLLISION_BOTTOM) {
    //        std::cout << "Collision on the bottom side!" << std::endl;
    //    }
    //}
    
        //void Movement_System::check_grounded(EntityID entityID) {
    //    auto& transform = ECSM.get_component<Transform2D>(entityID);
    //    auto& velocity = ECSM.get_component<Velocity_Component>(entityID);

    //    // Assume ground level is defined somewhere in your game logic
    //    const float groundLevel = 0.0f; // or your specific ground level

    //    // If position is below ground level and velocity is close to zero
    //    if (transform.position.y <= groundLevel && std::abs(velocity.velocity.y) < 0.1f) {
    //        std::cout << "Entity " << entityID << " is on the ground.\n";
    //        // Perform grounded actions or states here
    //    }
    //}

} // end of namespace lof
#endif
