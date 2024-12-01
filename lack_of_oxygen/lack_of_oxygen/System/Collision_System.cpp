/**
 * @file Collision_Syetem.cpp
 * @brief Implements the collsion system.
 * @author Saw Hui Shan (86%), Simon (3%), Ash (11%)
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
#include "../System/GUI_System.h"
#include "../Manager/ECS_Manager.h"
#include "../Utility/Constant.h"
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

        min.x = transform.prev_position.x - (collision.width / 2.0f);
        min.y = transform.prev_position.y - (collision.height / 2.0f);

        max.x = transform.prev_position.x + (collision.width / 2.0f);
        max.y = transform.prev_position.y + (collision.height / 2.0f);

        return AABB(min, max);
    }

    Collision_System::Collision_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Transform2D>()); 
        signature.set(ECSM.get_component_id<Collision_Component>()); 
        signature.set(ECSM.get_component_id<Physics_Component>()); 
        signature.set(ECSM.get_component_id<Velocity_Component>()); 

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
            return false; //no intersection
        }
      
        float Vb_x = vel2.x - vel1.x;; //initialize Vb for x axis 
        float Vb_y = vel2.y - vel1.y;; //initialize Vb for y axis
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


    void Collision_System::collision_check_scene1(std::vector<CollisionPair>& collisions, float delta_time) {
        // Iterate over entities matching the system's signature
        const auto& collision_entities = get_entities();

        for (auto it_1 = collision_entities.begin(); it_1 != collision_entities.end(); ++it_1) {
            EntityID entity_ID1 = *it_1;

            auto& physic1 = ECSM.get_component<Physics_Component>(entity_ID1);

            // Skip if entity is static
            if (physic1.get_is_static())
                continue;

            auto& transform1 = ECSM.get_component<Transform2D>(entity_ID1);
            auto& collision1 = ECSM.get_component<Collision_Component>(entity_ID1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(entity_ID1);
           
             // Create AABB for object 1
            AABB aabb1 = AABB::from_transform(transform1, collision1);

            bool is_grounded = false; // Track if entity is grounded

            auto it_2 = std::next(it_1); // Start from the next entity

            // Check for collisions with other entities
            for (auto iter_2 = collision_entities.begin(); iter_2 != collision_entities.end(); ++iter_2) {
                EntityID entity_ID2 = *iter_2;

                if (entity_ID1 == entity_ID2)
                {
                    continue;
                }

                //auto& physic2 = ECSM.get_component<Physics_Component>(entity_ID2);

                auto& transform2 = ECSM.get_component<Transform2D>(entity_ID2);
                auto& collision2 = ECSM.get_component<Collision_Component>(entity_ID2);
                auto& velocity2 = ECSM.get_component<Velocity_Component>(entity_ID2);


                if (!collision2.collidable) continue;


                // Create AABB for object 2
                AABB aabb2 = AABB::from_transform(transform2, collision2);

                // Check for intersection between two entities
                float collision_time = delta_time;
                if (collision_intersection_rect_rect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time, delta_time)) {

                    CollisionSide side = compute_collision_side(aabb1, aabb2);

                    if (side == CollisionSide::BOTTOM)
                    {
                        is_grounded = true;
                        physic1.set_gravity(Vec2D(0.0f, 0.0f));
                    }

                    //std::cout << "entity 2 is :" << entity_ID2 << "\n";
                    // Store collision pair and overlap information
                    collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2), side });
                    //std::cout << "Entity " << entity_ID1 << " collides with Entity " << entity_ID2 << " on side: " << static_cast<int>(side) << "\n";
                    //std::cout << "Entity " << entity_ID2 << " Position: (" << transform2.position.x << ", " << transform2.position.y << ")\n";
                    //std::cout << "State of is grouded for player: " << physic1.is_grounded << "\n";
                    //std::cout << "Entity " << entity_ID2 << " Position: (" << transform2.position.x << ", " << transform2.position.y << ")\n";
                }
            }
            physic1.set_is_grounded(is_grounded);
            if (!is_grounded) {
                physic1.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
            }
        }
    }

   // Initialize value for detect the tiles that player is near to
    EntityID Collision_System::bottom_collision_entity = static_cast<EntityID>(-1);
    bool Collision_System::has_bottom_collision = false;

    EntityID Collision_System::left_collision_entity = static_cast<EntityID>(-1);
    bool Collision_System::has_left_collision = false;

    EntityID Collision_System::right_collision_entity = static_cast<EntityID>(-1);
    bool Collision_System::has_right_collision = false;

    EntityID Collision_System::top_collision_entity = static_cast<EntityID>(-1);
    bool Collision_System::has_top_collision = false;

    void Collision_System::collision_check_scene2(std::vector<CollisionPair>& collisions, float delta_time) {

        // initialize value 
        bool found_bottom_collision = false;
        bool found_left_collision = false;
        bool found_right_collision = false;
        bool found_top_collision = false;

        const auto& collision_entities = get_entities();
        bool is_grounded = false;
        frame_counter++;

        EntityID current_bottom_entity = static_cast<EntityID>(-1);
        EntityID current_left_entity = static_cast<EntityID>(-1);
        EntityID current_right_entity = static_cast<EntityID>(-1);
        EntityID current_top_entity = static_cast<EntityID>(-1);

        // Level grid constants
        const float LEFT_BOUND = -960.0f; // follow the serialization file create_level_entities()
        const float RIGHT_BOUND = 960.0f; // follow the serialization file create_level_entities()
        const float START_Y = -150.0f; // follow the serialization file create_level_entities()
        const int TOTAL_ROWS = static_cast<int>(SM.get_level_rows()); 
        const int TOTAL_COLS = static_cast<int>(SM.get_level_cols());

        // Calculate cell dimensions based on level size
        const float CELL_WIDTH = (RIGHT_BOUND - LEFT_BOUND) / TOTAL_COLS;
        const float CELL_HEIGHT = CELL_WIDTH;

        for (auto iter1 = collision_entities.begin(); iter1 != collision_entities.end(); ++iter1) {
            EntityID entity_ID1 = *iter1;
            auto& physic1 = ECSM.get_component<Physics_Component>(entity_ID1);

            if (physic1.get_is_static()) {
                continue;
            }

            auto& transform1 = ECSM.get_component<Transform2D>(entity_ID1);
            auto& collision1 = ECSM.get_component<Collision_Component>(entity_ID1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(entity_ID1);

            // Convert world coordinate to grid coordinate
            int player_col = static_cast<int>((transform1.position.x - LEFT_BOUND) / CELL_WIDTH); // calculate the cols that player at 
            int player_row = static_cast<int>((START_Y - transform1.position.y) / CELL_HEIGHT); // calculate the rows that player at 


            // Ensure coordinate is within the range
            player_col = std::clamp(player_col, 0, TOTAL_COLS - 1); 
            player_row = std::clamp(player_row, 0, TOTAL_ROWS - 1);

            //// Debug output for player position
            //LM.write_log("\n=== Collision Check Debug ===");
            //printf("Player grid position: [Row: %d, Col: %d]\n", player_row, player_col);

            
            const int CHECK_RADIUS = 1; // 1 cell away from player 
            int start_row = std::max(0, player_row - CHECK_RADIUS); //get the starting rows that will be check always 1 rows before the player 
            int end_row = std::min(TOTAL_ROWS - 1, player_row + CHECK_RADIUS); //get the starting rows that will be check always 1 rows after the player


            int start_col = std::max(0, player_col - CHECK_RADIUS); // get the starting cols, always check 1 cols before the player
            int end_col = std::min(TOTAL_COLS - 1, player_col + CHECK_RADIUS); // get the ending cols, always check 1 cols before the player

            // Debug output for check area
            //LM.write_log("\nChecking area:");
            //printf("Rows: %d to %d\n", start_row, end_row);
            //printf("Cols: %d to %d\n", start_col, end_col);


            AABB aabb1 = AABB::from_transform(transform1, collision1);

            // only allow to check when the player is near the level design map
            if (transform1.position.y <= (START_Y + (collision1.height / 2.0f)))
            {
                // Check only entities in the surrounding cells
                for (auto iter2 = collision_entities.begin(); iter2 != collision_entities.end(); ++iter2) {
                    EntityID entity_ID2 = *iter2;

                    if (entity_ID1 == entity_ID2) {
                        continue;
                    }

                    auto& transform2 = ECSM.get_component<Transform2D>(entity_ID2);

                    // Calculate entity2's grid position
                    int entity2_col = static_cast<int>((transform2.position.x - LEFT_BOUND) / CELL_WIDTH);
                    int entity2_row = static_cast<int>((START_Y - transform2.position.y) / CELL_HEIGHT);

                    // Skip if entity2 is outside check area
                    if (entity2_row < start_row || entity2_row > end_row ||
                        entity2_col < start_col || entity2_col > end_col) {
                        continue;
                    }

                    auto& collision2 = ECSM.get_component<Collision_Component>(entity_ID2);
                    auto& velocity2 = ECSM.get_component<Velocity_Component>(entity_ID2);

                    // skip if component for collidable set to false
                    if (!collision2.collidable) continue;

                    AABB aabb2 = AABB::from_transform(transform2, collision2);

                    float collision_time = delta_time;
                    if (collision_intersection_rect_rect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time, delta_time)) {
                        CollisionSide side = compute_collision_side(aabb1, aabb2);

                        if (side == CollisionSide::BOTTOM && !is_grounded) {
                            //printf("this logic is occur \n");
                            is_grounded = true;
                            physic1.set_gravity(Vec2D(0.0f, 0.0f));
                            found_bottom_collision = true;
                            current_bottom_entity = entity_ID2;


                        }
                        else {
                            has_bottom_collision = false;
                            is_grounded = false;
                            bottom_collision_entity = static_cast<EntityID>(-1);
                        }

                        //printf("Collision detected: Entity %d with Entity %d at side %d\n", entity_ID1, entity_ID2, side);
                        //printf("this is frame counter: %f\n" , static_cast<float>(frame_counter));
                        //printf("this is bool of is_grounded %d\n", is_grounded);
                        collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2), side,  static_cast<float>(frame_counter) });

                    }

                    if (!found_left_collision &&
                        entity2_col == player_col - 1 && // one column to the left
                        entity2_row == player_row && // same row
                        transform2.position.x < transform1.position.x && // entity2 is actually to the left
                        std::abs(transform2.position.x - transform1.position.x) <= (CELL_WIDTH * 1.5f)) { // increased detection range 
                        found_left_collision = true;
                        current_left_entity = entity_ID2;
                        LM.write_log("Left collision detected: Entity %d at col %d", entity_ID2, entity2_col);
                    }

                    // Right check 
                    if (!found_right_collision &&
                        entity2_col == player_col + 1 && // one column to the right
                        entity2_row == player_row && // same row
                        transform2.position.x > transform1.position.x && // entity2 is actually to the right
                        std::abs(transform2.position.x - transform1.position.x) <= (CELL_WIDTH * 1.5f)) { // increased detection range 
                        found_right_collision = true;
                        current_right_entity = entity_ID2;
                        LM.write_log("Right collision detected: Entity %d at col %d", entity_ID2, entity2_col);
                    }

                    // Top check 
                    if (!found_top_collision &&
                        entity2_row == player_row - 1 && // Exactly one row above
                        entity2_col == player_col && // ensure it is same row
                        std::abs(transform2.position.y - transform1.position.y) < (CELL_HEIGHT * 1.2f)) {
                        found_top_collision = true;
                        current_top_entity = entity_ID2;
                        //LM.write_log("Top entity detected in row %d: %d", entity2_row, entity_ID2);
                    }

                    if (player_col <= 0 || player_col >= TOTAL_COLS - 1) {
                        
                        float leftmost_tile_center = LEFT_BOUND + (CELL_WIDTH * 0.5f);
                        float rightmost_tile_center = RIGHT_BOUND - (CELL_WIDTH * 0.5f);

                        if (transform1.position.x <= leftmost_tile_center) {
                            velocity1.velocity.x = 0.0f;
                            transform1.position.x = leftmost_tile_center;
                        }
                        else if (transform1.position.x >= rightmost_tile_center) {
                            velocity1.velocity.x = 0.0f;
                            transform1.position.x = rightmost_tile_center;
                        }
                    }

                    // Bottom check (only work if level_design map first row is empty 'e')
                    //if (!found_bottom_collision &&
                    //    entity2_row == player_row + 1 && // Exactly one row below
                    //    entity2_col == player_col && // ensure it is same row
                    //    std::abs(transform2.position.y - transform1.position.y) < (CELL_HEIGHT * 1.2f)) {
                    //    found_bottom_collision = true;
                    //    current_bottom_entity = entity_ID2;
                    //    //LM.write_log("Top entity detected in row %d: %d", entity2_row, entity_ID2);
                    //}

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
            bottom_collision_entity = static_cast<EntityID>(-1);
            has_bottom_collision = false;
        }

        // Left collision
        if (found_left_collision) {
            left_collision_entity = current_left_entity;
            has_left_collision = true;
            //std::cout << left_collision_entity << "\n";
        }
        else {
            left_collision_entity = static_cast<EntityID>(-1);
            has_left_collision = false;
        }

        // Right collision
        if (found_right_collision) {
            right_collision_entity = current_right_entity;
            has_right_collision = true;
        }
        else {
            right_collision_entity = static_cast<EntityID>(-1);
            has_right_collision = false;
        }
        if (found_top_collision) {
            top_collision_entity = current_top_entity;
            has_top_collision = true;
        }
        else {
            top_collision_entity = static_cast<EntityID>(-1);
            has_top_collision = false;
        }
    }

    EntityID Collision_System::check_non_collidable_entities = static_cast<EntityID>(-1);
    EntityID Collision_System::mineral_tank = static_cast<EntityID>(-1);
    EntityID Collision_System::oxygen_tank = static_cast<EntityID>(-1);
    bool Collision_System::entites_detect = false;

    void Collision_System::Colliside_Oxygen_Mineral(float delta_time)
    {
        const auto& collision_entities = get_entities();

        for (auto iter1 = collision_entities.begin(); iter1 != collision_entities.end(); ++iter1)
        {
            EntityID player_ID = *iter1;
            auto& physic1 = ECSM.get_component<Physics_Component>(player_ID);

            if (physic1.get_is_static()) {
                continue;
            }

            auto& player_transform = ECSM.get_component<Transform2D>(player_ID);
            auto& player_collision1 = ECSM.get_component<Collision_Component>(player_ID);
            auto& player_velocity1 = ECSM.get_component<Velocity_Component>(player_ID);
            // std::cout << "Entity " << entity_ID1 << " Position: (" << transform1.position.x << ", " << transform1.position.y << ")\n";
             // Create AABB for object 1
            AABB aabb_player = AABB::from_transform(player_transform, player_collision1);

            //bool is_grounded = false; // Track if entity is grounded

            auto it_2 = std::next(iter1); // Start from the next entity

            // Check for collisions with other entities
            for (auto iter2 = collision_entities.begin(); iter2 != collision_entities.end(); ++iter2) {
                EntityID entities_ID = *iter2;

                if (player_ID == entities_ID)
                {
                    continue;
                }

                auto& entities_transform = ECSM.get_component<Transform2D>(entities_ID);
                auto& entities_collision = ECSM.get_component<Collision_Component>(entities_ID);
                auto& entities_velocity = ECSM.get_component<Velocity_Component>(entities_ID);

                if (entities_collision.collidable) continue;

                AABB enttities_aabb = AABB::from_transform(entities_transform, entities_collision);
                if (entities_ID == 3) {
                    enttities_aabb.max.x += 40.0f; // Extend right side by 20 units as the asset centre affected the detected area
                }

                float collision_time = delta_time;
                if (collision_intersection_rect_rect(aabb_player, player_velocity1.velocity, enttities_aabb, entities_velocity.velocity, collision_time, delta_time)) {
                    check_non_collidable_entities = entities_ID;

                    entites_detect = true;
                    break;
                }
                else
                {
                    check_non_collidable_entities = static_cast<EntityID>(-1);
                    entites_detect = false;
                }
            }

            if (check_non_collidable_entities == 2)
            {
                mineral_tank = check_non_collidable_entities;
            }
            else if (check_non_collidable_entities == 3)
            {
                oxygen_tank = check_non_collidable_entities;
            }
            else {
                oxygen_tank = static_cast<EntityID>(-1);
                mineral_tank = static_cast<EntityID>(-1);
            }


        }

        // Find GUI System to trigger interface
        for (auto& system : ECSM.get_systems()) {
            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                // Check mineral tank collision
                if (mineral_tank_detected() != -1) {
                    gui_system->show_mineral_tank_gui();
                }
                else {
                    gui_system->hide_mineral_tank_gui();
                }

                // Check oxygen tank collision
                if (oxygen_tank_detected() != -1) {
                    gui_system->show_oxygen_tank_gui();
                }
                else {
                    gui_system->hide_oxygen_tank_gui();
                }

                break;
            }
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


    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
        // Store the last frame's collision states
        static bool had_left_collision = false;
        static bool had_right_collision = false;
        static float last_collision_time = 0.0f;
        static const float COLLISION_MEMORY_TIME = 0.1f;

        // Constants for collision response tuning
        const float PENETRATION_TOLERANCE = 0.005f;   // Reduced from 0.01f for less sinking
        const float CORRECTION_FACTOR = 0.15f;        // Reduced from 0.2f for gentler correction
        const float VELOCITY_DAMPING = 0.8f;          // Added to smooth out collision responses
        //const float MIN_VELOCITY = 0.01f;             // Threshold for zeroing out small velocities

        // Update collision states
        for (const auto& collision : collisions) {

            if (collision.side == CollisionSide::LEFT) {
                had_left_collision = true;
                last_collision_time = collision.collide_time; 
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

                    float minimal_correction = 0.05f; // Reduced from 0.1f
                    // Minimal position correction to prevent sticking
                    float correction_x = (collision.side == CollisionSide::LEFT) ? minimal_correction : -minimal_correction;
                    transformA.position.x += correction_x;
                    continue;
                }

                // Normal collision handling
                Vec2D relative_velocity = velocityA.velocity * VELOCITY_DAMPING;

                float restitution = (collision.side == CollisionSide::BOTTOM) ? 0.0f : 0.05f; //reduced from 0.1f to 0.05f

                float impulse_scalar = -(1.0f + restitution) * dot_product_vec2d(relative_velocity, normal);
                impulse_scalar /= physicsA.get_inv_mass();
                Vec2D impulse = normal * impulse_scalar;
                velocityA.velocity += impulse * physicsA.get_inv_mass();

  
                Vec2D correction(0.0f, 0.0f);

                if (collision.overlap.y > PENETRATION_TOLERANCE) {

                    float correction_magnitude = (collision.overlap.y - PENETRATION_TOLERANCE) * CORRECTION_FACTOR;
                    correction = normal * correction_magnitude;

                    if (collision.side == CollisionSide::BOTTOM) {
                        correction.x = 0.0f;
                        physicsA.set_is_grounded(true);
                        physicsA.set_has_jumped(false);
                        physicsA.set_gravity(Vec2D(0.0f, 0.0f));

                        //zero out very small vertical velocity
                        if (std::abs(velocityA.velocity.y) < 0.1f) {
                            velocityA.velocity.y = 0.0f;
                        }
                    }
                    //for side collisions, reduce horizontal correction
                    else if (collision.side == CollisionSide::LEFT ||
                        collision.side == CollisionSide::RIGHT) {
                        correction *= 0.05f; //reduce horizontal push back
                    }
                }

                transformA.position += correction;

                //additional velocity dampening for horizontal collisions
                if (collision.side == CollisionSide::LEFT ||
                    collision.side == CollisionSide::RIGHT) {
                    velocityA.velocity.x *= 0.9f; //reduce horizontal velocity
                }
            }
        }
    }


    void Collision_System::update(float delta_time) {
        std::vector<CollisionPair> collisions;
        //std::cout << "---------------------------this is check collide in collision syystem----------------------------------------\n";
        collision_check_collide(collisions, delta_time); // Check for collisions and fill the collision list
        //printf("this is bool of is_collide_bottom %d\n", is_collide_bottom);
        // printf("this is the entity that get %d\n", get_bottom_collide_entity());
        //printf("entity bottom: %d\n", has_bottom_collide_detect());

        //printf("this is the entity that get %d\n", get_bottom_collide_entity());
        // printf("entity bottom: %d\n", has_bottom_collide_detect());
        //EntityID test = get_left_collide_entity();

        Colliside_Oxygen_Mineral(delta_time);

  

        //sprintf("this is the entity left %d\n", test);
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


