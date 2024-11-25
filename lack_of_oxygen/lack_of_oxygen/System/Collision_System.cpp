/**
 * @file Collision_Syetem.cpp
 * @brief Implements the collsion system.
 * @author Saw Hui Shan (100%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.

 */



 // Include standard headers
#include <iostream>


// Include headers
#include "Collision_System.h"

// Include other necessary headers
#include "../Component/Component.h"
#include "../System/Render_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Utility/Constant.h"
#include "../Main/Main.h" // for extern
#include "../Manager/Input_Manager.h"
#include "../Utility/Entity_Selector_Helper.h"
#include "../Manager/Serialization_Manager.h"

namespace lof {
    std::unique_ptr<Collision_System> Collision_System::instance;
    std::once_flag Collision_System::once_flag;
    

   

    Collision_System& Collision_System::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Collision_System);
            });
        return *instance;
    }

 

    AABB::AABB(const Vec2D& min, const Vec2D& max)
        : min(min), max(max) {}

    AABB AABB::from_transform(const Transform2D& transform, const Collision_Component& collision) {
        Vec2D min;
        Vec2D max;
#if 1
        min.x = transform.prev_position.x - (collision.width / 2.0f);
        min.y = transform.prev_position.y - (collision.height / 2.0f);

        max.x = transform.prev_position.x + (collision.width / 2.0f);
        max.y = transform.prev_position.y + (collision.height / 2.0f);

        LM.write_log("Creating AABB for entity at (%.2f, %.2f) with size (%.2f, %.2f)",
            transform.prev_position.x, transform.prev_position.y,
            collision.width, collision.height);
#endif
#if 0
        // Use the actual world position
        float world_x = transform.position.x;  // or prev_position.x if that's what you need
        float world_y = transform.position.y;  // or prev_position.y if that's what you need

        // Calculate AABB bounds using world position
        min.x = world_x - (collision.width / 2.0f);
        min.y = world_y - (collision.height / 2.0f);
        max.x = world_x + (collision.width / 2.0f);
        max.y = world_y + (collision.height / 2.0f);

        // Debug print to verify calculations
        LM.write_log("AABB Calculation:");
        LM.write_log("World Position: (%.2f, %.2f)", world_x, world_y);
        LM.write_log("Collision Size: (%.2f, %.2f)", collision.width, collision.height);
        LM.write_log("Calculated AABB: min(%.2f, %.2f), max(%.2f, %.2f)",
            min.x, min.y, max.x, max.y);
#endif 
        return AABB(min, max);
    }

    Collision_System::Collision_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Transform2D>()); // simon
        signature.set(ECSM.get_component_id<Collision_Component>()); // simon
        signature.set(ECSM.get_component_id<Physics_Component>()); // simon
        signature.set(ECSM.get_component_id<Velocity_Component>()); // simon

        LM.write_log("Collision_System initialized with signature requiring Transform2D, Collision_Component, Physics_Component, and Velocity_Component."); //simon
    }

    std::string Collision_System::get_type() const {
        return "Collision_System";
    }

    bool Collision_System::collision_intersection_rect_rect(const AABB& aabb1,
        const Vec2D& vel1,
        const AABB& aabb2,
        const Vec2D& vel2,
        float& firstTimeOfCollision,
        float delta_time) {
        // Check for AABB intersection
        // Check for AABB intersection
        //check for A.max < B.min or A.min > B.min for x axis & y axis
        if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x ||
            aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y)
        {
            //LM.write_log("Early exit: No intersection");
            return false; //no intersection
        }
        // LM.write_log("Checking collision with velocities - V1(%.2f, %.2f), V2(%.2f, %.2f)",
            // vel1.x, vel1.y, vel2.x, vel2.y);
        float Vb_x = vel2.x - vel1.x;; //initialize Vb for x axis 
        float Vb_y = vel2.y - vel1.y;; //initialize Vb for y axis
        //LM.write_log("Relative velocity: Vb(%.2f, %.2f)", Vb_x, Vb_y);
        float dFirst_x = aabb1.max.x - aabb2.min.x;  //initialize for dFirst for x-axis
        float dFirst_y = aabb1.max.y - aabb2.min.y;; // initialize for dFrist for y axis
        float dLast_x = aabb1.min.x - aabb2.max.x;; //initialize for dLast for x axis
        float dLast_y = aabb1.min.y - aabb2.max.y;; //initialize for dLast for y axis

        float tFirst = firstTimeOfCollision = 0.0f; //Initialise t first
        float tLast = delta_time; //initialize tLast and assume 1 as the time step (g_dt)

        //for x-axis
        if (Vb_x < 0)
        {
            //case 1 for x-axis
            if (aabb1.min.x > aabb2.max.x)
            {
                return false;
            }
            //case 4 for x-axis
            if (aabb1.max.x < aabb2.min.x)
            {
                if (tFirst < dFirst_x / Vb_x)
                {
                    tFirst = dFirst_x / Vb_x;
                }
            }

            if (aabb1.min.x < aabb2.max.x)
            {
                if (tLast > dLast_x / Vb_x)
                {
                    tLast = dLast_x / Vb_x;
                }
            }
        }
        else if (Vb_x > 0)
        {
            //case 2 for x_axis
            if (aabb1.min.x > aabb2.max.x)
            {
                if (tFirst < dLast_x / Vb_x)
                {
                    tFirst = dLast_x / Vb_x;
                }
            }
            if (aabb1.max.x > aabb2.min.x)
            {
                if (tLast > dFirst_x / Vb_x)
                {
                    tLast = dFirst_x / Vb_x;
                }
            }

            //case 3
            if (aabb1.max.x < aabb2.min.x)
            {
                return false;
            }

        }
        else if (Vb_x == 0) {
            //case 5;
            if (aabb1.max.x < aabb2.min.x)
            {
                return false;
            }
            else if (aabb1.min.x > aabb2.max.x)
            {
                return false;
            }
        }

        //for y-axis
        if (Vb_y < 0)
        {
            //case 1 for x-axis
            if (aabb1.min.y > aabb2.max.y)
            {
                return 0;
            }
            //case 4 for x-axis
            if (aabb1.max.y < aabb2.min.y)
            {
                if (tFirst < dFirst_y / Vb_y)
                {
                    tFirst = dFirst_y / Vb_y;
                }
            }
            if (aabb1.min.y < aabb2.max.y)
            {
                if (tLast > dLast_y / Vb_y)
                {
                    tLast = dLast_y / Vb_y;
                }
            }
        }
        else if (Vb_y > 0)
        {
            //case 2 for x_axis
            if (aabb1.min.y > aabb2.max.y)
            {
                if (tFirst < dLast_y / Vb_y)
                {
                    tFirst = dLast_y / Vb_y;
                }
            }
            if (aabb1.max.y > aabb2.min.y)
            {
                if (tLast > dFirst_y / Vb_y)
                {
                    tLast = dFirst_y / Vb_y;
                }
            }
            //case 3
            if (aabb1.max.y < aabb2.min.y)
            {
                return false;
            }
        }
        else if (Vb_y == 0) {
            //case 5;
            if (aabb1.max.y < aabb2.min.y)
            {
                return false;
            }
            else if (aabb1.min.y > aabb2.max.y)
            {
                return false;
            }
        }
        //case 6; 
        if (tFirst > tLast)
        {
            return false;
        }
        return true; //the rectangle intersect

}

    Vec2D Collision_System::compute_overlap(const AABB& aabb1, const AABB& aabb2) {
        float overlap_x = std::min(aabb1.max.x, aabb2.max.x) - std::max(aabb1.min.x, aabb2.min.x);
        float overlap_y = std::min(aabb1.max.y, aabb2.max.y) - std::max(aabb1.min.y, aabb2.min.y);

        // Ensure overlaps are non-negative
        overlap_x = std::max(0.0f, overlap_x);
        overlap_y = std::max(0.0f, overlap_y);
        //std::cout << "Overlap X: " << overlap_x << ", Overlap Y: " << overlap_y << std::endl;


        return Vec2D(overlap_x, overlap_y);
    }

    std::string Collision_System::collisionSideToString(CollisionSide side) {
        switch (side) {
        case CollisionSide::LEFT:
            return "LEFT";
        case CollisionSide::RIGHT:
            return "RIGHT";
        case CollisionSide::TOP:
            return "TOP";
        case CollisionSide::BOTTOM:
            return "BOTTOM";
        default:
            return "NONE";
        }
    }

    CollisionSide Collision_System::compute_collision_side(const AABB& aabb1, const AABB& aabb2) {
        // Calculate the center points of each AABB
        Vec2D center1 = { (aabb1.min.x + aabb1.max.x) / 2, (aabb1.min.y + aabb1.max.y) / 2 };
        Vec2D center2 = { (aabb2.min.x + aabb2.max.x) / 2, (aabb2.min.y + aabb2.max.y) / 2 };

        // Calculate the distances
        float dx = center1.x - center2.x;
        float dy = center1.y - center2.y;

        // Calculate half extents
        float halfWidth1 = (aabb1.max.x - aabb1.min.x) / 2.0f;
        float halfHeight1 = (aabb1.max.y - aabb1.min.y) / 2.0f;
        float halfWidth2 = (aabb2.max.x - aabb2.min.x) / 2.0f;
        float halfHeight2 = (aabb2.max.y - aabb2.min.y) / 2.0f;

        // Calculate the overlap on both axes
        float overlapX = halfWidth1 + halfWidth2 - std::abs(dx);
        float overlapY = halfHeight1 + halfHeight2 - std::abs(dy);

        // Determine the side of the collision
        CollisionSide side = CollisionSide::NONE;
        // Early exit for no collision
        const float EPSILON = 0.001f;
        if (overlapX <= EPSILON || overlapY <= EPSILON) {
            return CollisionSide::NONE; // No collision
        }


        if (overlapX >= 0 && overlapY >= 0) {
            if (overlapX < overlapY) {
                side = (dx > 0) ? CollisionSide::LEFT : CollisionSide::RIGHT;
            }
            else {
                side = (dy > 0) ? CollisionSide::BOTTOM : CollisionSide::TOP;
            }
        }
        //std::cout << "Collision detected on side: " << collisionSideToString(side) << std::endl;

        //return CollisionSide::NONE; // No collision
        return side;
    }

    void Collision_System::collision_check_collide(std::vector<CollisionPair>& collisions, float delta_time) {
        // Get current scene number
        int current_scene = SM.scene_switch();

        if (current_scene == 1) {
            collision_check_scene1(collisions, delta_time);
        }
        else if (current_scene == 2) {
            collision_check_scene2(collisions, delta_time);
        }
    }

#if 1// original code
    void Collision_System::collision_check_scene1(std::vector<CollisionPair>& collisions, float delta_time) {
        // Iterate over entities matching the system's signature
        const auto& collision_entities = get_entities();
        //std::cout << "Total entities: " << entities.size() << "\n";

        for (auto it_1 = collision_entities.begin(); it_1 != collision_entities.end(); ++it_1) {
            EntityID entity_ID1 = *it_1;

            auto& physic1 = ECSM.get_component<Physics_Component>(entity_ID1);


            // Skip if entity is static
            if (physic1.get_is_static())
                continue;


            auto& transform1 = ECSM.get_component<Transform2D>(entity_ID1);
            auto& collision1 = ECSM.get_component<Collision_Component>(entity_ID1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(entity_ID1);
            // std::cout << "Entity " << entity_ID1 << " Position: (" << transform1.position.x << ", " << transform1.position.y << ")\n";
             // Create AABB for object 1
            AABB aabb1 = AABB::from_transform(transform1, collision1);

            bool is_grounded = false; // Track if entity is grounded

            auto it_2 = std::next(it_1); // Start from the next entity

            // Check for collisions with other entities
            for (auto it_2 = collision_entities.begin(); it_2 != collision_entities.end(); ++it_2) {
                EntityID entity_ID2 = *it_2;
                //std::cout << "entity 1 is :" << entity_ID1 << "\n";

                if (entity_ID1 == entity_ID2)
                {
                    continue;
                }


                //auto& physic2 = ECSM.get_component<Physics_Component>(entity_ID2);

                auto& transform2 = ECSM.get_component<Transform2D>(entity_ID2);
                auto& collision2 = ECSM.get_component<Collision_Component>(entity_ID2);
                auto& velocity2 = ECSM.get_component<Velocity_Component>(entity_ID2);


                // Create AABB for object 2
                AABB aabb2 = AABB::from_transform(transform2, collision2);



                // Check for intersection between two entities
                float collision_time = delta_time;
                if (collision_intersection_rect_rect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time, delta_time)) {
                    //LM.write_log("Player is collide with something checck\n");

                    CollisionSide side = compute_collision_side(aabb1, aabb2);

                    if (side == CollisionSide::BOTTOM)
                    {
                        is_grounded = true;
                        //LM.write_log("is on a platform now ");
                        //std::cout << "this is collide top\n";
                        physic1.set_gravity(Vec2D(0.0f, 0.0f));
                    }


                    // Store collision pair and overlap information
                    collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2), side });
                    //std::cout << "Entity " << entity_ID1 << " collides with Entity " << entity_ID2 << " on side: " << static_cast<int>(side) << "\n";
                    //std::cout << "Entity " << entity_ID1 << " collides with Entity " << entity_ID2 << " on side: " << static_cast<int>(side) << "\n";

                    //std::cout << "Entity " << entity_ID1 << " Position: (" << transform1.position.x << ", " << transform1.position.y << ")\n";
                    //std::cout << "Entity " << entity_ID2 << " Position: (" << transform2.position.x << ", " << transform2.position.y << ")\n";
                    //std::cout << "State of is grouded for player: " << physic1.is_grounded << "\n";

                   // std::cout << "Entity " << entity_ID1 << " Position: (" << transform1.position.x << ", " << transform1.position.y << ")\n";
                    //std::cout << "Entity " << entity_ID2 << " Position: (" << transform2.position.x << ", " << transform2.position.y << ")\n";
                    //std::cout << "State of is grouded for player: " << physic1.get_is_grounded() << "\n";
              

                }
            }
            physic1.set_is_grounded(is_grounded);
            if (!is_grounded) {
                physic1.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
            }
        }
    }
#endif 

    //bool Collision_System::collision_handled = false;
    EntityID Collision_System::bottom_collision_entity = -1;
    bool Collision_System::has_bottom_collision = false;

    EntityID Collision_System::left_collision_entity = -1;
    bool Collision_System::has_left_collision = false;

    EntityID Collision_System::right_collision_entity = -1;
    bool Collision_System::has_right_collision = false;

    void Collision_System::collision_check_scene2(std::vector<CollisionPair>& collisions, float delta_time) {
 
        bool found_bottom_collision = false;
        bool found_left_collision = false;
        bool found_right_collision = false;
        const auto& collision_entities = get_entities();
        bool is_grounded = false;
        frame_counter++;
    
        EntityID current_bottom_entity = -1;
        EntityID current_left_entity = -1;
        EntityID current_right_entity = -1;

        // Level grid constants
        const float LEFT_BOUND = -1020.0f;
        const float RIGHT_BOUND = 1020.0f;
        const float START_Y = -150.0f;
        const int TOTAL_ROWS = SM.get_level_rows();
        const int TOTAL_COLS = SM.get_level_cols();

        // Calculate cell dimensions based on level size
        const float CELL_WIDTH = (RIGHT_BOUND - LEFT_BOUND) / TOTAL_COLS;
        const float CELL_HEIGHT = CELL_WIDTH; // Assuming square cells

        for (auto it_1 = collision_entities.begin(); it_1 != collision_entities.end(); ++it_1) {
            EntityID entity_ID1 = *it_1;
            auto& physic1 = ECSM.get_component<Physics_Component>(entity_ID1);

            if (physic1.get_is_static()) {
                continue;
            }

            auto& transform1 = ECSM.get_component<Transform2D>(entity_ID1);
            auto& collision1 = ECSM.get_component<Collision_Component>(entity_ID1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(entity_ID1);

          
            // Calculate player's position in the level grid
            int player_col = static_cast<int>((transform1.position.x - LEFT_BOUND) / CELL_WIDTH);
            int player_row = static_cast<int>((START_Y - transform1.position.y) / CELL_HEIGHT);

            player_col = std::clamp(player_col, 0, TOTAL_COLS - 1); 
            player_row = std::clamp(player_row, 0, TOTAL_ROWS - 1);  

            //// Debug output for player position
            //LM.write_log("\n=== Collision Check Debug ===");
            LM.write_log("Player (Entity %d) world position: (%.2f, %.2f)",
                entity_ID1, transform1.position.x, transform1.position.y);
            //LM.write_log("Player grid position: [Row: %d, Col: %d]", player_row, player_col);
            //printf("Player grid position: [Row: %d, Col: %d]\n", player_row, player_col);

            // Define the 5x5 check area around the player
            const int CHECK_RADIUS = 1; // 2 cells in each direction + current cell = 5x5
            int start_row = std::max(0, player_row - CHECK_RADIUS); //get the starting rows that will be check always 2 rows before the player
            int end_row = std::min(TOTAL_ROWS - 1, player_row + CHECK_RADIUS); 
         

            int start_col = std::max(0, player_col - CHECK_RADIUS); // get the starting cols, always check 2 cols before the player
            int end_col = std::min(TOTAL_COLS - 1, player_col + CHECK_RADIUS);

            // Debug output for check area
            //LM.write_log("\nChecking area:");
            // printf("Rows: %d to %d\n", start_row, end_row);
            // printf("Cols: %d to %d\n", start_col, end_col);
          

            AABB aabb1 = AABB::from_transform(transform1, collision1);
            
            if (transform1.position.y <= (START_Y + (collision1.height / 2.0f)))
            {
                // Check only entities in the surrounding cells
                for (auto it_2 = collision_entities.begin(); it_2 != collision_entities.end(); ++it_2) {
                    EntityID entity_ID2 = *it_2;

                    if (entity_ID1 == entity_ID2) {
                        continue;
                    }

                    auto& transform2 = ECSM.get_component<Transform2D>(entity_ID2);

                    // Calculate entity2's grid position
                    int entity2_col = static_cast<int>((transform2.position.x - LEFT_BOUND) / CELL_WIDTH);
                    int entity2_row = static_cast<int>((START_Y - transform2.position.y) / CELL_HEIGHT);

                    // Skip if entity2 is outside our 5x5 check area
                    if (entity2_row < start_row || entity2_row > end_row ||
                        entity2_col < start_col || entity2_col > end_col) {
                        continue;
                    }

                    auto& collision2 = ECSM.get_component<Collision_Component>(entity_ID2);
                    auto& velocity2 = ECSM.get_component<Velocity_Component>(entity_ID2);

                    AABB aabb2 = AABB::from_transform(transform2, collision2);

                    float collision_time = delta_time;
                    if (collision_intersection_rect_rect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time, delta_time)) {
                        CollisionSide side = compute_collision_side(aabb1, aabb2);

#if 0
                        if (side == CollisionSide::BOTTOM && !is_grounded) {
                            //printf("this logic is occur \n");
                            is_grounded = true;
                            physic1.set_gravity(Vec2D(0.0f, 0.0f));
                            found_bottom_collision = true;
                            current_bottom_entity = entity_ID2;
                            found_left_collision = false;
                            found_right_collision = false;
                           
                        }
                        else {
                            has_bottom_collision = false;
                            is_grounded = false;
                            bottom_collision_entity = -1;
                        }
#endif
                        switch (side) {
                        case CollisionSide::BOTTOM:
                            if (!is_grounded) {
                                is_grounded = true;
                                physic1.set_gravity(Vec2D(0.0f, 0.0f));
                                found_bottom_collision = true;
                                current_bottom_entity = entity_ID2;
                            }
                            break;

                        case CollisionSide::LEFT:
                            found_left_collision = true;
                            current_left_entity = entity_ID2;
                            break;

                        case CollisionSide::RIGHT:
                            found_right_collision = true;
                            current_right_entity = entity_ID2;
                            break;
                        }
                        
                        //printf("Collision detected: Entity %d with Entity %d at side %d\n", entity_ID1, entity_ID2, side);
                        //printf("this is frame counter: %f\n" , static_cast<float>(frame_counter));
                        //printf("this is bool of is_grounded %d\n", is_grounded);
                        
                       
                        collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2), side,  static_cast<float>(frame_counter) });
                      
                    }
                }
            } //end of -150

            physic1.set_is_grounded(is_grounded);
            if (!is_grounded) {
                physic1.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
            }
        }

        if (found_bottom_collision) {
            bottom_collision_entity = current_bottom_entity;
            has_bottom_collision = true;
        }
        else {
            bottom_collision_entity = -1;
            has_bottom_collision = false;
        }

        // Left collision
        if (found_left_collision) {
            left_collision_entity = current_left_entity;
            has_left_collision = true;
        }
        else {
            left_collision_entity = -1;
            has_left_collision = false;
        }

        // Right collision
        if (found_right_collision) {
            right_collision_entity = current_right_entity;
            has_right_collision = true;
        }
        else {
            right_collision_entity = -1;
            has_right_collision = false;
        }
    }
   

 /**
 * @brief Resolves collisions between entities based on the provided collision pairs.
 *
 * This function processes each collision in the list of `CollisionPair` objects, applying
 * appropriate collision response for dynamic entities colliding with static entities. It
 * calculates the impulse to prevent overlapping and corrects positions to avoid "sinking"
 * into static objects. For bottom collisions, it manages the grounded state and
 * stabilizes velocity to prevent jitter.
 *
 * @param collisions A vector of `CollisionPair` objects representing collisions between entities.
 */
#if 0///original
    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
        for (const auto& collision : collisions) {
            EntityID entityA = collision.entity1;
            EntityID entityB = collision.entity2;

            auto& physicsA = ECSM.get_component<Physics_Component>(entityA);
            auto& transformA = ECSM.get_component<Transform2D>(entityA);
            auto& velocityA = ECSM.get_component<Velocity_Component>(entityA);
           
            // Only process dynamic entities colliding with static entities
            if (!physicsA.get_is_static()) {
                Vec2D normal(0.0f, 0.0f);
                if (collision.side == CollisionSide::LEFT) normal = Vec2D(1.0f, 0.0f);
                else if (collision.side == CollisionSide::RIGHT) normal = Vec2D(-1.0f, 0.0f);
                else if (collision.side == CollisionSide::TOP) normal = Vec2D(0.0f, -1.0f);
                else if (collision.side == CollisionSide::BOTTOM) normal = Vec2D(0.0f, 1.0f);

                Vec2D relative_velocity = velocityA.velocity;

                // Apply minimal restitution for bottom collisions to prevent bouncing
                float restitution = (collision.side == CollisionSide::BOTTOM) ? 0.0f : 0.1f;

                // Compute impulse scalar
                float impulse_scalar = -(1.0f + restitution) * dot_product_vec2d(relative_velocity, normal);
                impulse_scalar /= physicsA.get_inv_mass();

                // Apply impulse
                Vec2D impulse = normal * impulse_scalar;
                velocityA.velocity += impulse * physicsA.get_inv_mass();

                // Position correction to avoid sinking
                float percent = 0.2f;  // Adjust as needed
                float slop = 0.01f;    // Allowable penetration
                Vec2D correction = normal * std::max((collision.overlap.y - slop) * percent, 0.0f);

                // Only apply correction on the y-axis for bottom collisions
                if (collision.side == CollisionSide::BOTTOM) {
                    correction.x = 0.0f;  // Prevent horizontal corrections
                }

                transformA.position += correction;

                // Additional handling for grounded state and velocity stabilization
                if (collision.side == CollisionSide::BOTTOM) {
                    physicsA.set_is_grounded(true);
                    physicsA.set_has_jumped(false);
                    physicsA.set_gravity(Vec2D(0.0f, 0.0f));

                    // Dampen velocity if close to zero to prevent jitter
                    if (std::abs(velocityA.velocity.y) < 0.1f) {
                        velocityA.velocity.y = 0.0f;
                    }
                }
            }
        }
    }


#endif
   
#if 1 // new one 

    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
        // Store the last frame's collision states
        static bool had_left_collision = false;
        static bool had_right_collision = false;
        static float last_collision_time = 0.0f;
        static const float COLLISION_MEMORY_TIME = 0.1f; 


        // Update collision states
        for (const auto& collision : collisions) {
            if (collision.side == CollisionSide::LEFT) {
                had_left_collision = true;
                last_collision_time = collision.collide_time; // Or your game's time system
            }
            if (collision.side == CollisionSide::RIGHT) {
                had_right_collision = true;
                last_collision_time = collision.collide_time;
            }
        }

        // Reset states if too much time has passed
       
        if (frame_counter - last_collision_time > COLLISION_MEMORY_TIME) {
            had_left_collision = false;
            had_right_collision = false;
        }

        // Check for vertical gap scenario
        bool is_in_vertical_gap = had_left_collision && had_right_collision;

        // Process collisions
        for (const auto& collision : collisions) {
            EntityID entityA = collision.entity1;
            auto& physicsA = ECSM.get_component<Physics_Component>(entityA);
            auto& transformA = ECSM.get_component<Transform2D>(entityA);
            auto& velocityA = ECSM.get_component<Velocity_Component>(entityA);

            if (!physicsA.get_is_static()) {
                Vec2D normal(0.0f, 0.0f);
                if (collision.side == CollisionSide::LEFT) normal = Vec2D(1.0f, 0.0f);
                else if (collision.side == CollisionSide::RIGHT) normal = Vec2D(-1.0f, 0.0f);
                else if (collision.side == CollisionSide::TOP) normal = Vec2D(0.0f, -1.0f);
                else if (collision.side == CollisionSide::BOTTOM) normal = Vec2D(0.0f, 1.0f);

                if (is_in_vertical_gap && (collision.side == CollisionSide::LEFT || collision.side == CollisionSide::RIGHT) && (SM.scene_switch() == 2)) {
                    // Special handling for vertical gap
                    velocityA.velocity.x = 0.0f;
                    physicsA.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                    physicsA.set_is_grounded(false);

                    // Minimal position correction to prevent sticking
                    float correction_x = (collision.side == CollisionSide::LEFT) ? 0.1f : -0.1f;
                    transformA.position.x += correction_x;
                    continue;
                }

                // Normal collision handling
                Vec2D relative_velocity = velocityA.velocity;
                float restitution = (collision.side == CollisionSide::BOTTOM) ? 0.0f : 0.1f;
                float impulse_scalar = -(1.0f + restitution) * dot_product_vec2d(relative_velocity, normal);
                impulse_scalar /= physicsA.get_inv_mass();
                Vec2D impulse = normal * impulse_scalar;
                velocityA.velocity += impulse * physicsA.get_inv_mass();

                float percent = 0.2f;
                float slop = 0.01f;
                Vec2D correction = normal * std::max((collision.overlap.y - slop) * percent, 0.0f);

                if (collision.side == CollisionSide::BOTTOM) {
                    correction.x = 0.0f;
                    physicsA.set_is_grounded(true);
                    physicsA.set_has_jumped(false);
                    physicsA.set_gravity(Vec2D(0.0f, 0.0f));

                    if (std::abs(velocityA.velocity.y) < 0.1f) {
                        velocityA.velocity.y = 0.0f;
                    }
                }

                transformA.position += correction;
            }
        }
    }
#endif 
  

#if 0
    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
    // Group collisions by entity
    std::map<EntityID, std::vector<CollisionPair>> collision_groups;
    for (const auto& collision : collisions) {
        collision_groups[collision.entity1].push_back(collision);
    }

    // Process each entity's collisions
    for (const auto& [entityA, entity_collisions] : collision_groups) {
        auto& physicsA = ECSM.get_component<Physics_Component>(entityA);
        auto& transformA = ECSM.get_component<Transform2D>(entityA);
        auto& velocityA = ECSM.get_component<Velocity_Component>(entityA);

        // Check for simultaneous left-right collision
        bool has_left = false;
        bool has_right = false;
        for (const auto& collision : entity_collisions) {
            if (collision.side == CollisionSide::LEFT) has_left = true;
            if (collision.side == CollisionSide::RIGHT) has_right = true;
        }
        bool is_squeezed = has_left && has_right;

        // Process each collision
        for (const auto& collision : entity_collisions) {
            if (!physicsA.get_is_static()) {
                Vec2D normal(0.0f, 0.0f);
                if (collision.side == CollisionSide::LEFT) normal = Vec2D(1.0f, 0.0f);
                else if (collision.side == CollisionSide::RIGHT) normal = Vec2D(-1.0f, 0.0f);
                else if (collision.side == CollisionSide::TOP) normal = Vec2D(0.0f, -1.0f);
                else if (collision.side == CollisionSide::BOTTOM) normal = Vec2D(0.0f, 1.0f);

                Vec2D relative_velocity = velocityA.velocity;
                float restitution = (collision.side == CollisionSide::BOTTOM) ? 0.0f : 0.1f;

                float impulse_scalar = -(1.0f + restitution) * dot_product_vec2d(relative_velocity, normal);
                impulse_scalar /= physicsA.get_inv_mass();
                Vec2D impulse = normal * impulse_scalar;

                // Handle different collision cases
                if (is_squeezed) {
                    // Only stop horizontal movement when squeezed
                    velocityA.velocity.x = 0.0f;
                    // Maintain vertical movement
                    physicsA.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                }
                else if (collision.side == CollisionSide::TOP) {
                    velocityA.velocity.y = 0.0f;
                    physicsA.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                    physicsA.set_is_grounded(false);
                }
                else {
                    velocityA.velocity += impulse * physicsA.get_inv_mass();
                }

                // Position correction
                float percent = 0.2f;
                float slop = 0.01f;
                Vec2D correction;
                
                if (is_squeezed) {
                    // Only apply vertical correction when squeezed
                    correction = Vec2D(0.0f, normal.y * std::max((collision.overlap.y - slop) * percent, 0.0f));
                } else {
                    correction = normal * std::max((collision.overlap.y - slop) * percent, 0.0f);
                }

                if (collision.side == CollisionSide::BOTTOM) {
                    correction.x = 0.0f;
                    physicsA.set_is_grounded(true);
                    physicsA.set_has_jumped(false);
                    physicsA.set_gravity(Vec2D(0.0f, 0.0f));

                    if (std::abs(velocityA.velocity.y) < 0.1f) {
                        velocityA.velocity.y = 0.0f;
                    }
                }

                transformA.position += correction;
            }
        }
    }
}
    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
    // Group collisions by entity
    std::map<EntityID, std::vector<CollisionPair>> collision_groups;
    for (const auto& collision : collisions) {
        collision_groups[collision.entity1].push_back(collision);
    }

    // Process each entity's collisions
    for (const auto& [entityA, entity_collisions] : collision_groups) {
        auto& physicsA = ECSM.get_component<Physics_Component>(entityA);
        auto& transformA = ECSM.get_component<Transform2D>(entityA);
        auto& velocityA = ECSM.get_component<Velocity_Component>(entityA);

        // Check for simultaneous left-right collision
        bool has_left = false;
        bool has_right = false;
        for (const auto& collision : entity_collisions) {
            if (collision.side == CollisionSide::LEFT) has_left = true;
            if (collision.side == CollisionSide::RIGHT) has_right = true;
        }
        bool is_squeezed = has_left && has_right;

        // Process each collision
        for (const auto& collision : entity_collisions) {
            if (!physicsA.get_is_static()) {
                Vec2D normal(0.0f, 0.0f);
                if (collision.side == CollisionSide::LEFT) normal = Vec2D(1.0f, 0.0f);
                else if (collision.side == CollisionSide::RIGHT) normal = Vec2D(-1.0f, 0.0f);
                else if (collision.side == CollisionSide::TOP) normal = Vec2D(0.0f, -1.0f);
                else if (collision.side == CollisionSide::BOTTOM) normal = Vec2D(0.0f, 1.0f);

                Vec2D relative_velocity = velocityA.velocity;
                float restitution = (collision.side == CollisionSide::BOTTOM) ? 0.0f : 0.1f;

                float impulse_scalar = -(1.0f + restitution) * dot_product_vec2d(relative_velocity, normal);
                impulse_scalar /= physicsA.get_inv_mass();
                Vec2D impulse = normal * impulse_scalar;

                // Handle different collision cases
                if (is_squeezed) {
                    velocityA.velocity.x = 0.0f;
                    // Allow vertical movement when squeezed
                    if (velocityA.velocity.y > 0) {
                        physicsA.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                    }
                } else {
                    // Grid alignment based on collision side
                    switch (collision.side) {
                        case CollisionSide::BOTTOM:
                            transformA.position.y = std::floor(transformA.position.y) + 0.5f;
                            break;
                        case CollisionSide::TOP:
                            transformA.position.y = std::ceil(transformA.position.y) - 0.5f;
                            velocityA.velocity.y = 0.0f;
                            physicsA.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                            break;
                        case CollisionSide::LEFT:
                            transformA.position.x = std::ceil(transformA.position.x) - 0.5f;
                            break;
                        case CollisionSide::RIGHT:
                            transformA.position.x = std::floor(transformA.position.x) + 0.5f;
                            break;
                    }
                    velocityA.velocity += impulse * physicsA.get_inv_mass();
                }

                // Position correction
                float percent = 0.2f;
                float slop = 0.02f;
                Vec2D correction;
                
                if (is_squeezed) {
                    // Only apply vertical correction when squeezed
                    correction = Vec2D(0.0f, normal.y * std::max((collision.overlap.y - slop) * percent, 0.0f));
                } else if (collision.side == CollisionSide::LEFT || collision.side == CollisionSide::RIGHT) {
                    correction = normal * std::max((collision.overlap.x - slop) * percent, 0.0f);
                } else {
                    correction = normal * std::max((collision.overlap.y - slop) * percent, 0.0f);
                }

                if (collision.side == CollisionSide::BOTTOM) {
                    correction.x = 0.0f;
                    physicsA.set_is_grounded(true);
                    physicsA.set_has_jumped(false);
                    physicsA.set_gravity(Vec2D(0.0f, 0.0f));

                    if (std::abs(velocityA.velocity.y) < 0.1f) {
                        velocityA.velocity.y = 0.0f;
                    }
                } else if (collision.side == CollisionSide::TOP) {
                    correction.x = 0.0f;
                    velocityA.velocity.y = 0.1f;
                }

                transformA.position += correction;

                // Dampen horizontal velocity
                if ((collision.side == CollisionSide::LEFT || collision.side == CollisionSide::RIGHT)
                    && std::abs(velocityA.velocity.x) < 0.1f) {
                    velocityA.velocity.x = 0.0f;
                }
            }
        }
    }
}

#endif 



    void Collision_System::update(float delta_time) {
        std::vector<CollisionPair> collisions;
        //std::cout << "---------------------------this is check collide in collision syystem----------------------------------------\n";
        collision_check_collide(collisions, delta_time); // Check for collisions and fill the collision list
        //printf("this is bool of is_collide_bottom %d\n", is_collide_bottom);
        // printf("this is the entity that get %d\n", get_bottom_collide_entity());
        //printf("entity bottom: %d\n", has_bottom_collide_detect());

        //printf("this is the entity that get %d\n", get_bottom_collide_entity());
        // printf("entity bottom: %d\n", has_bottom_collide_detect());

        //printf("this is the entity left %d\n", get_left_collide_entity());
        //printf("entity left: %d\n", has_left_collide_detect());
        //

        //printf("this is the entity right %d\n", get_right_collide_entity());
        //printf("entity right: %d\n", has_right_collide_detect());

        //std::cout << "---------------------------this is end of check collide in collision syystem----------------------------------------\n";
        resolve_collision_event(collisions);
        //Check_Selected_Entity();
     
        // std::cout << "collision time: " << delta_time << "\n";
        // std::cout << SM.scene_switch() << "in game mode\n";
    }

    bool Collision_System::isInterseptBox(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
    {

        
        return (mouseX > (box_x - width / 2.0f) && mouseX < (box_x + width / 2.0f) &&
            mouseY >(box_y - height / 2.0f) && mouseY < (box_y + height / 2.0f));
        
    }




} // namespace lof


