/**
 * @file Collision_Syetem.cpp
 * @brief Implements the collsion system.
 * @author Saw Hui Shan (77%), Simon (3%), Wai Lwin Thit (20%)
 * @date September 21, 2024
 * Copyright (C) 2025 DigiPen Institute of Technology.
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
#include "../Utility/Globals.h"
#include "../Manager/Input_Manager.h"
#include "../Utility/Entity_Selector_Helper.h"
#include "../Manager/Serialization_Manager.h"
#include "../Manager/Audio_Manager.h"
#include "../Manager/IMGUI_Manager.h"

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

    /*
    PointLine::PointLine(const Vec2D& center, const Vec2D& edge) 
        : center(center), edge(edge){}
    //to be emerged from the player 
    PointLine PointLine::create_Line(const Transform2D& transform, const CollisionSide side, const Collision_Component& collision) {
        Vec2D center {transform.position.x, transform.position.y};
        Vec2D edge{ center };

        switch (side) {
        case CollisionSide::LEFT : 
            edge.x -= collision.width;  
            break; 
        case CollisionSide::RIGHT : 
            edge.x += collision.width; 
            break; 
        case CollisionSide::BOTTOM : 
            edge.y -= collision.height;
            break; 
        case CollisionSide::TOP : 
            edge.y += collision.height;
            break;
        }
       return PointLine(center, edge); 
    }
    */

    Collision_System::Collision_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Transform2D>()); 
        signature.set(ECSM.get_component_id<Collision_Component>()); 
        signature.set(ECSM.get_component_id<Physics_Component>()); 
        signature.set(ECSM.get_component_id<Velocity_Component>()); 
        //LM.write_log("Collision_System initialized with signature requiring Transform2D, Collision_Component, Physics_Component, and Velocity_Component."); //simon
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
        int current_scene = GM.get_current_scene();

      /*  if (current_scene == 1) {
            collision_check_scene1(collisions, delta_time);
        }*/
        if (current_scene == 2) {
            collision_check_scene2(collisions, delta_time);
        }
    }



    /*
    * @brief Checks the entity's vent type, stores the direction and return the vent direction type
    */

    VentDirection Collision_System::get_vent_direction(EntityID id) const {
        auto* entity = ECSM.get_entity(id);
        if (!entity) return VentDirection::NONE;

        //get the name
        const std::string& name = entity->get_name();

        //check for vents 

        //up air vent
        if (name.find("ventUp_prefab") != std::string::npos ||
            name.find("ventStripUp_prefab") != std::string::npos) {
            return VentDirection::UP;
        }
        //left air vents
        else if (name.find("ventLeft_prefab") != std::string::npos ||
            name.find("ventStripLeft_prefab") != std::string::npos) {
            return VentDirection::LEFT;
        }
        //right air vents
        else if (name.find("ventRight_prefab") != std::string::npos ||
            name.find("ventStripRight_prefab") != std::string::npos) {
            return VentDirection::RIGHT;
        }
        return VentDirection::NONE;
    }

    bool is_fade_active() {
        for (auto& system : ECSM.get_systems()) {
            if (auto* gui_sys = dynamic_cast<GUI_System*>(system.get())) {

                return gui_sys->is_fade_active();

            }
        }
    }

    void Collision_System::apply_vent_force(EntityID id, VentDirection direction, bool found_next_vent) {

        auto& e_physics = ECSM.get_component<Physics_Component>(id);
        auto& e_velocity = ECSM.get_component<Velocity_Component>(id);
        EntityID playerID = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);

        //separate audio state tracking for each type
        static bool is_in_up_vent = false;
        static bool is_in_left_vent = false;
        static bool is_in_right_vent = false;

        bool& is_in_vent = (direction == VentDirection::UP) ? is_in_up_vent :
            (direction == VentDirection::LEFT) ? is_in_left_vent :
            is_in_right_vent;

        //apply force based on direction 

        switch (direction) {

        case VentDirection::UP: {
            //apply up movement
            e_physics.force_helper.activate_force(VENT_FORCE);
            break;
        }
        case VentDirection::LEFT: {
            //update player animation 
            int& player_direction = GFXM.get_player_direction();
            player_direction = FACE_LEFT;
            int& moving_status = GFXM.get_moving_status();
            moving_status = RUN_LEFT;

            //apply force
            e_physics.force_helper.activate_force(MOVE_LEFT);
            break;
        }
        case VentDirection::RIGHT: {
            //Update player animation
            int& player_direction = GFXM.get_player_direction();
            player_direction = FACE_RIGHT;
            int& moving_status = GFXM.get_moving_status();
            moving_status = RUN_RIGHT;

            //apply force
            e_physics.force_helper.activate_force(MOVE_RIGHT);
            break;
        }
        default:
            return;
        }

        //handle audio 
        //audio for vent in 
        if (!is_in_vent) {

            if (!GM.get_player_dead_state()) {
                ADM.play_now(playerID, "air vent in", ECSM.get_component<Audio_Component>(playerID));
            }

            is_in_vent = true;

        }

        if (!found_next_vent) {
            //vent exit 
            if (direction == VentDirection::UP) {
                // e_physics.force_helper.deactivate_force(VENT_FORCE); 
            }

            if (is_in_vent) {

                //if (!GM.get_player_dead_state()) {
                //    ADM.stop_now(playerID, "air vent in", ECSM.get_component<Audio_Component>(playerID).get_filepath("air vent in"));
                //    ADM.play_now(playerID, "air vent out", ECSM.get_component<Audio_Component>(playerID));
                //}

                bool fade_active = is_fade_active();

                if (!GM.get_player_dead_state() && !fade_active) {
                    ADM.stop_now(playerID, "air vent in", ECSM.get_component<Audio_Component>(playerID).get_filepath("air vent in"));
                    ADM.play_now(playerID, "air vent out", ECSM.get_component<Audio_Component>(playerID));
                }

                if (fade_active) {

                    ADM.pause_group(GroupType::TYPE_BGM);
                    ADM.pause_group(GroupType::TYPE_SFX);

                    //ADM.stop_now(playerID, "air vent in", ECSM.get_component<Audio_Component>(playerID).get_filepath("air vent in"));
                    //ADM.stop_now(playerID, "air vent out", ECSM.get_component<Audio_Component>(playerID).get_filepath("air vent out"));
                }

                is_in_vent = false;
            }
        }

        std::cout << "Player in the air vent? " << is_in_vent << std::endl;
    }




    void Collision_System::handle_vent_collision(EntityID entity, EntityID vent, float delta_time, bool& is_grounded) {
        // Get components for entity
        auto& e_physics = ECSM.get_component<Physics_Component>(entity);
        auto& e_velocity = ECSM.get_component<Velocity_Component>(entity);
        auto& e_transform = ECSM.get_component<Transform2D>(entity);
        auto& e_collision = ECSM.get_component<Collision_Component>(entity);

        // Vent components
        auto& av_transform = ECSM.get_component<Transform2D>(vent);
        auto& av_collision = ECSM.get_component<Collision_Component>(vent);
        auto& av_velocity = ECSM.get_component<Velocity_Component>(vent);

        //get vent direction 
        VentDirection vent_direction = get_vent_direction(vent);
        if (vent_direction == VentDirection::NONE) return;

        // AABB for intersection check
        AABB e_aabb = AABB::from_transform(e_transform, e_collision);
        AABB av_aabb = AABB::from_transform(av_transform, av_collision);

        // Level grid constants
        const float LEFT_BOUND = -960.0f;
        const float RIGHT_BOUND = 960.0f;
        const float START_Y = -150.0f;
        const int TOTAL_ROWS = static_cast<int>(SM.get_level_rows());
        const int TOTAL_COLS = static_cast<int>(SM.get_level_cols());
        const float CELL_WIDTH = (RIGHT_BOUND - LEFT_BOUND) / TOTAL_COLS;
        const float CELL_HEIGHT = CELL_WIDTH;

        // Get player's grid position
        int player_col = static_cast<int>((e_transform.position.x - LEFT_BOUND) / CELL_WIDTH);
        int player_row = static_cast<int>((START_Y - e_transform.position.y) / CELL_HEIGHT);

            // Check for entity in the tile above using the top collision detection logic
            player_col = std::clamp(player_col, 0, TOTAL_COLS - 1);
            player_row = std::clamp(player_row, 0, TOTAL_ROWS - 1);

        // Check for collision
        float collision_time = delta_time;

        if (collision_intersection_rect_rect(e_aabb, e_velocity.velocity, av_aabb, av_velocity.velocity,
            collision_time, delta_time)) {


            //calculate overlaps
            Vec2D overlap = compute_overlap(e_aabb, av_aabb);
            bool found_next_vent = false;

                if (vent_direction == VentDirection::UP) {

                    for (EntityID check_id : get_entities()) {
                        if (entity == check_id || vent == check_id) continue;

                        auto& check_transform = ECSM.get_component<Transform2D>(check_id);

                        //convert to grit position
                        int entity2_col = static_cast<int>((check_transform.position.x - LEFT_BOUND) / CELL_WIDTH);
                        int entity2_row = static_cast<int>((START_Y - check_transform.position.y) / CELL_HEIGHT);

                        //check if in grid position above player
                        if (entity2_row == player_row - 1 && entity2_col == player_col) {
                            if (get_vent_direction(check_id) == VentDirection::UP) {
                                found_next_vent = true;
                                break;
                            }
                        }
                    }

                   //static bool is_in_air_vent = false;
                    // Calculate horizontal center distance between player and vent
                    float horizontal_distance = std::abs(e_transform.position.x - av_transform.position.x);
                    //bool has_vertical_overlap = overlap.y > 0;


                    if (horizontal_distance <= av_collision.width / 2.0f) {

                        apply_vent_force(entity, vent_direction, found_next_vent);

                    }
                    else {
                        e_physics.force_helper.deactivate_force(VENT_FORCE);
                        if (!is_grounded) {
                           e_physics.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                        }
                    }

                }
                else if (vent_direction == VentDirection::LEFT) {
                    for (EntityID check_id : get_entities()) {
                        if (entity == check_id || vent == check_id) continue;

                        auto& check_transform = ECSM.get_component<Transform2D>(check_id);

                        // Convert to grid position
                        int entity2_col = static_cast<int>((check_transform.position.x - LEFT_BOUND) / CELL_WIDTH);
                        int entity2_row = static_cast<int>((START_Y - check_transform.position.y) / CELL_HEIGHT);

                        // Check if in grid position to the left of player
                        if (entity2_col == player_col - 1 && entity2_row == player_row) {
                            if (get_vent_direction(check_id) == VentDirection::LEFT) {
                                found_next_vent = true;
                                break;
                            }
                        }
                    }

                    float vertical_distance = std::abs(e_transform.position.y - av_transform.position.y);
                    if (vertical_distance <= av_collision.height / 5.0f) {
                        apply_vent_force(entity, vent_direction, found_next_vent);
                    }
                }

                else if (vent_direction == VentDirection::RIGHT) {
                    for (EntityID check_id : get_entities()) {
                        if (entity == check_id || vent == check_id) continue;

                        auto& check_transform = ECSM.get_component<Transform2D>(check_id);

                        // Convert to grid position
                        int entity2_col = static_cast<int>((check_transform.position.x - LEFT_BOUND) / CELL_WIDTH);
                        int entity2_row = static_cast<int>((START_Y - check_transform.position.y) / CELL_HEIGHT);

                        // Check if in grid position to the right of player
                        if (entity2_col == player_col + 1 && entity2_row == player_row) {
                            if (get_vent_direction(check_id) == VentDirection::RIGHT) {
                                found_next_vent = true;
                                break;
                            }
                        }
                    }

                    float vertical_distance = std::abs(e_transform.position.y - av_transform.position.y);
                    if (vertical_distance <= av_collision.height / 5.0f) {
                        apply_vent_force(entity, vent_direction, found_next_vent);
                    }
                }
                
            
        }
}



    //void Collision_System::collision_check_scene1(std::vector<CollisionPair>& collisions, float delta_time) {
    //    // Iterate over entities matching the system's signature
    //    const auto& collision_entities = get_entities();

    //    for (auto it_1 = collision_entities.begin(); it_1 != collision_entities.end(); ++it_1) {
    //        EntityID entity_ID1 = *it_1;

    //        auto& physic1 = ECSM.get_component<Physics_Component>(entity_ID1);

    //        // Skip if entity is static
    //        if (physic1.get_is_static())
    //            continue;

    //        auto& transform1 = ECSM.get_component<Transform2D>(entity_ID1);
    //        auto& collision1 = ECSM.get_component<Collision_Component>(entity_ID1);
    //        auto& velocity1 = ECSM.get_component<Velocity_Component>(entity_ID1);
    //       
    //         // Create AABB for object 1
    //        AABB aabb1 = AABB::from_transform(transform1, collision1);

    //        bool is_grounded = false; // Track if entity is grounded

    //        auto it_2 = std::next(it_1); // Start from the next entity

    //        // Check for collisions with other entities
    //        for (auto iter_2 = collision_entities.begin(); iter_2 != collision_entities.end(); ++iter_2) {
    //            EntityID entity_ID2 = *iter_2;

    //            if (entity_ID1 == entity_ID2)
    //            {
    //                continue;
    //            }

    //            //auto& physic2 = ECSM.get_component<Physics_Component>(entity_ID2);

    //            auto& transform2 = ECSM.get_component<Transform2D>(entity_ID2);
    //            auto& collision2 = ECSM.get_component<Collision_Component>(entity_ID2);
    //            auto& velocity2 = ECSM.get_component<Velocity_Component>(entity_ID2);


    //            if (!collision2.collidable) continue;


    //            // Create AABB for object 2
    //            AABB aabb2 = AABB::from_transform(transform2, collision2);

    //            // Check for intersection between two entities
    //            float collision_time = delta_time;
    //            if (collision_intersection_rect_rect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time, delta_time)) {

    //                CollisionSide side = compute_collision_side(aabb1, aabb2);

    //                if (side == CollisionSide::BOTTOM)
    //                {
    //                    is_grounded = true;
    //                    physic1.set_gravity(Vec2D(0.0f, 0.0f));
    //                }

    //                
    //                // Store collision pair and overlap information
    //                collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2), side });
    //        
    //            }
    //        }
    //        physic1.set_is_grounded(is_grounded);
    //        if (!is_grounded) {
    //            physic1.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
    //        }
    //    }
    //}


    void Collision_System::Boundary_Check() {
        GLfloat screen_width = static_cast<GLfloat>(SM.get_scr_width());

        auto& camera = GFXM.get_camera();
        GLfloat camera_x = camera.pos_x;

        const auto& collision_entities = get_entities();

        for (auto iter1 = collision_entities.begin(); iter1 != collision_entities.end(); ++iter1) {
            EntityID player_ID = *iter1;
            auto& physic1 = ECSM.get_component<Physics_Component>(player_ID);

            // Skip static entities (not moving)
            if (physic1.get_is_static()) {
                continue;
            }

            auto& player_transform = ECSM.get_component<Transform2D>(player_ID);
            auto& player_velocity = ECSM.get_component<Velocity_Component>(player_ID);

            // Calculate half-width of the player
            GLfloat player_half_width = player_transform.scale.x / 2.0f;

            // Calculate boundaries based on the camera's position
            GLfloat half_width = screen_width / 2.0f;

            GLfloat minX = -half_width + camera_x + player_half_width;
            GLfloat maxX = half_width + camera_x - player_half_width;

            // Stop the player when reaching the left or right boundary
            if (player_transform.position.x <= minX && player_velocity.velocity.x < 0) {
                player_velocity.velocity.x = 0.0f;
                player_transform.position.x = minX;
            }
            else if (player_transform.position.x >= maxX && player_velocity.velocity.x > 0) {
                player_velocity.velocity.x = 0.0f;
                player_transform.position.x = maxX;
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



#if 1

    void Collision_System::collision_check_scene2(std::vector<CollisionPair>& collisions, float delta_time) {
        // Initialize collision detection flags
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
        const float LEFT_BOUND = -960.0f;
        const float RIGHT_BOUND = 960.0f;
        const float START_Y = -150.0f;
        const int TOTAL_ROWS = static_cast<int>(SM.get_level_rows());
        const int TOTAL_COLS = static_cast<int>(SM.get_level_cols());

        // Calculate cell dimensions based on level size
        const float CELL_WIDTH = (RIGHT_BOUND - LEFT_BOUND) / TOTAL_COLS;
        const float CELL_HEIGHT = CELL_WIDTH;

        const float SIDE_COLLISION_THRESHOLD = CELL_WIDTH * 1.1f; //reduced from 1.5f

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
            int player_col = static_cast<int>((transform1.position.x - LEFT_BOUND) / CELL_WIDTH);
            int player_row = static_cast<int>((START_Y - transform1.position.y) / CELL_HEIGHT);

            float first_row_y = START_Y; 
            bool is_above_first_row = transform1.position.y > first_row_y; 

            // Ensure coordinate is within the range
            player_col = std::clamp(player_col, 0, TOTAL_COLS - 1);
            player_row = std::clamp(player_row, 0, TOTAL_ROWS - 1);

            const int CHECK_RADIUS = 1; // Check 1 cell away from player
            int start_row = std::max(0, player_row - CHECK_RADIUS);
            int end_row = std::min(TOTAL_ROWS - 1, player_row + CHECK_RADIUS);
            int start_col = std::max(0, player_col - CHECK_RADIUS);
            int end_col = std::min(TOTAL_COLS - 1, player_col + CHECK_RADIUS);

            AABB aabb1 = AABB::from_transform(transform1, collision1);

            //offset the player's y for the collision box
            aabb1.max.y -= 30; 

            // Only check collisions when player is near the level design map
            if (transform1.position.y <= (START_Y + (collision1.height / 2.0f))) {

                for (auto iter2 = collision_entities.begin(); iter2 != collision_entities.end(); ++iter2) {
                    EntityID entity_ID2 = *iter2;

                    if (entity_ID1 == entity_ID2) {
                        continue;
                    }

                    auto* entity2 = ECSM.get_entity(entity_ID2);
                    if (!entity2) continue;

                  

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


                    // Check if this is a vent and handle it separately
                    VentDirection vent_dir = get_vent_direction(entity_ID2);
                    if (vent_dir != VentDirection::NONE) {
                        handle_vent_collision(entity_ID1, entity_ID2, delta_time, is_grounded);
                        continue;
                    }

                    if (!collision2.collidable) continue; //skip non-collidable entities

                    AABB aabb2 = AABB::from_transform(transform2, collision2);

                    float collision_time = delta_time;
                    if (collision_intersection_rect_rect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time, delta_time)) {
                        CollisionSide side = compute_collision_side(aabb1, aabb2);

  
                            if (side == CollisionSide::BOTTOM && !is_grounded) {
                                is_grounded = true;
                                physic1.set_gravity(Vec2D(0.0f, 0.0f));
                                found_bottom_collision = true;
                                current_bottom_entity = entity_ID2;
                            }

                            collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2), side, static_cast<float>(frame_counter) });
                        
                    }
                   

                    if (!is_above_first_row) {
                    // Check for left collision
                        if (!found_left_collision &&
                            entity2_col == player_col - 1 &&
                            entity2_row == player_row &&
                            transform2.position.x < transform1.position.x &&
                            std::abs(transform2.position.x - transform1.position.x) <= SIDE_COLLISION_THRESHOLD) {
                            found_left_collision = true;
                            current_left_entity = entity_ID2;
                        }

                        // Check for right collision
                        if (!found_right_collision &&
                            entity2_col == player_col + 1 &&
                            entity2_row == player_row &&
                            transform2.position.x > transform1.position.x &&
                            std::abs(transform2.position.x - transform1.position.x) <= SIDE_COLLISION_THRESHOLD) {
                            found_right_collision = true;
                            current_right_entity = entity_ID2;
                        }

                    }
                    // Check for top collision
                    if (!found_top_collision &&
                        entity2_row == player_row - 1 &&
                        entity2_col == player_col &&
                        std::abs(transform2.position.y - transform1.position.y) < (CELL_HEIGHT * 1.2f)) {
                        found_top_collision = true;
                        current_top_entity = entity_ID2;
                    }

                    // Handle level boundaries
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
                }
            }

            // Update physics state based on collisions
            physic1.set_is_grounded(is_grounded);
            if (!is_grounded) {
                physic1.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
            }
        }

        // Update collision detection state
        if (found_bottom_collision) {
            bottom_collision_entity = current_bottom_entity;
            has_bottom_collision = true;
        }
        else {
            bottom_collision_entity = static_cast<EntityID>(-1);
            has_bottom_collision = false;
        }

        if (found_left_collision) {
            left_collision_entity = current_left_entity;
            has_left_collision = true;
        }
        else {
            left_collision_entity = static_cast<EntityID>(-1);
            has_left_collision = false;
        }

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


#endif

    EntityID Collision_System::check_non_collidable_entities = static_cast<EntityID>(-1);
    EntityID Collision_System::mineral_tank = static_cast<EntityID>(-1);
    EntityID Collision_System::oxygen_tank = static_cast<EntityID>(-1);
    bool Collision_System::entites_detect = false;
    int deposit_count = 0;
    //bool deposit_count_bool = false;
    int previous_minerals = 0;
    float previous_oxygen = 0;
    int oxygen_count = 0; 


    //bool increasing = false;
    bool bgm_oxygen_increasing = false;  // Separate flag for BGM state
    float last_oxygen_level = 0.0f;

    void Collision_System::Colliside_Oxygen_Mineral(float delta_time)
    {
       
        const auto& collision_entities = get_entities();

        for (auto iter1 = collision_entities.begin(); iter1 != collision_entities.end(); ++iter1)
        {
            e_last_frame = e_press;
            e_press = IM.is_key_held(GLFW_KEY_E);

            
            EntityID player_ID = *iter1;
            auto& physic1 = ECSM.get_component<Physics_Component>(player_ID);

            if (physic1.get_is_static()) {
                continue;
            }
           

            auto& player_transform = ECSM.get_component<Transform2D>(player_ID);
            auto& player_collision1 = ECSM.get_component<Collision_Component>(player_ID);
            auto& player_velocity1 = ECSM.get_component<Velocity_Component>(player_ID);

            AABB aabb_player = AABB::from_transform(player_transform, player_collision1);

            auto it_2 = std::next(iter1);

            std::string oxygen_tank_str = "Oxygen_Tank";
            std::string mineral_hoppper_str = "mineral_hopper_conveyor";
            EntityID oxygen_tank_entity = ECSM.find_entity_by_name(oxygen_tank_str);
            EntityID mineral_hopper_str = ECSM.find_entity_by_name(mineral_hoppper_str);

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
                
                if (entities_ID == oxygen_tank_entity) {
                    enttities_aabb.max.x += 40.0f; // Extend right side by 20 units as the asset centre affected the detected area
                    //std::cout << "yes!!!!\n";
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

            if (check_non_collidable_entities == mineral_hopper_str)
            {
                mineral_tank = check_non_collidable_entities;
            }
            else if (check_non_collidable_entities == oxygen_tank_entity)
            {
                oxygen_tank = check_non_collidable_entities;
            }
            else {
                oxygen_tank = static_cast<EntityID>(-1);
                mineral_tank = static_cast<EntityID>(-1);
            }
        }

        bool is_e_pressed = IM.is_key_pressed(GLFW_KEY_E);
        bool is_e_held = IM.is_key_held(GLFW_KEY_E);
        //bool is_e_release = IM.is_key_released(GLFW_KEY_E);
        
        // Find GUI System to trigger interface and handle mineral deposit
        for (auto& system : ECSM.get_systems()) {
            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                // Check mineral tank collision and handle deposit
                if (mineral_tank_detected() != -1) {
                    gui_system->show_mineral_tank_gui();
                   
                    // Debug the key state

                    // Try both pressed and held states
                    if (is_e_pressed || is_e_held) {
                        EntityID text_entity = ECSM.find_entity_by_name("top_ui_mineral_count_text");

                        std::string deposit_mineral_sound = "mineral deposit";

                        if (text_entity != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(text_entity)) {
                            auto& text_comp = ECSM.get_component<Text_Component>(text_entity);
                            //auto& player_audio = ECSM.get_component<Audio_Component>(player_entity);

                            try {
                                // Get current minerals from UI text
                                int current_minerals = std::stoi(text_comp.text);

                                if (current_minerals >= 100) {
                                    current_minerals -= 100;

                                    total_deposited_minerals += 100;
                                    previous_minerals = current_minerals; //store previous value
                                    if (previous_minerals -= 100) {
                                        deposit_count++;
                                    }

                                    // Calculate progress percentage based on total deposited minerals
                                    // This produces a value between 0.0 and 1.0
                                    float current_percentage = total_deposited_minerals / 50000.0f;
                                    current_percentage = std::min(current_percentage, 1.0f); // Clamp to 1.0, not 100.0

                                    // Update progress bar and its text
                                    gui_system->update_mineral_progress(current_percentage);

                                    // IMPORTANT: Directly update the top UI goal percentage text as well
                                    EntityID goal_text_id = ECSM.find_entity_by_name("top_ui_goal_percentage_text");
                                    if (goal_text_id != INVALID_ENTITY_ID && ECSM.has_component<Text_Component>(goal_text_id)) {
                                        auto& goal_text = ECSM.get_component<Text_Component>(goal_text_id);
                                        int percentage = static_cast<int>(current_percentage * 100.0f);
                                        std::stringstream ss;
                                        ss << std::setw(2) << std::setfill('0') << percentage << "%";
                                        goal_text.text = ss.str();

                                        // Log the update
                                        LM.write_log("Updated top UI goal text to %s", goal_text.text.c_str());
                                    }

                                    // Update the mineral count display
                                    text_comp.text = std::to_string(current_minerals);
                                }
                            }
                            catch (const std::exception& e) {
                                // Handle exception
                                LM.write_log("Error processing mineral deposit: %s", e.what());
                            }
                        }
                    }
                   
                    
                  

                    EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                    if ((is_e_pressed || is_e_held) && deposit_count > 0)
                    {
                        ADM.play_now(playerId, "deposit mineral", ECSM.get_component<Audio_Component>(playerId));
                        //ECSM.get_component<Audio_Component>(playerId).set_isactive("deposit_mineral", true);
                        //ECSM.get_component<Audio_Component>(playerId).increase_playcount("deposit mineral");
                        deposit_count--;
                    }
                    else if (!(e_press && e_last_frame))
                    {
                        ADM.stop_now(playerId, "mineral deposit", "sfx_mineral_deposit");
                    }
                }
                else {
                    gui_system->hide_mineral_tank_gui();
                    //deposit = false;
                }
                
                
              

                // Check oxygen tank collision
                if (oxygen_tank_detected() != -1)
                {
                    gui_system->show_oxygen_tank_gui();

                    // If E is pressed or held
                    //bool is_e_pressed = IM.is_key_pressed(GLFW_KEY_E);
                    //bool is_e_held = IM.is_key_held(GLFW_KEY_E);
                    bool increasing = false;

                    if (is_e_pressed || is_e_held)
                    {
                        // (1) Player oxygen / Ship oxygen
                        float playerOxy = GM.get_current_oxygen_level(); // [0..100]
                        float shipOxy = GM.get_ship_oxygen_level();    // [0..400]

                        // (2) If player not full and ship has some oxygen
                        if (playerOxy < 100.0f && shipOxy > 0.0f)
                        {
                            // (3) Figure out how much the player needs
                            float needed = 100.0f - playerOxy;
                            previous_oxygen = playerOxy;
                            // The ship can only give up to 'shipOxy' it has:
                            float transfer = std::min(needed, shipOxy);

                            // Transfer
                            //playerOxy += transfer;  // player goes up
                            //shipOxy -= transfer;  // ship goes down
                            
                            if (transfer > 0) {
                                playerOxy++; //player goes up
								shipOxy--; // ship goes down
                                increasing = true;
                            }
                            else {
                                increasing = false;
                            }

                            // (4) Store them back
                            GM.set_current_oxygen_level(playerOxy);
                            GM.set_ship_oxygen_level(shipOxy);

                            // (5) Update the GUI bars
                            //    - Player fraction = playerOxy / 100
                            float playerFraction = playerOxy / 100.0f;
                            gui_system->update_oxygen_progress1(playerFraction);


                            float usedFraction = (400.0f - shipOxy) / 400.0f;
                            gui_system->update_oxygen_progress2(usedFraction);

                            if (playerOxy > previous_oxygen && increasing) {
                                EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                                ADM.play_now(playerId, "refilling oxygen", ECSM.get_component<Audio_Component>(playerId));
                                //ECSM.get_component<Audio_Component>(playerId).set_isactive("refilling oxygen", true);
                                //ECSM.get_component<Audio_Component>(playerId).increase_playcount("refilling oxygen");
							}
                            else if (!increasing && !(e_press && e_last_frame)) {
                                EntityID playerId = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
                                ADM.stop_now(playerId, "refilling oxygen", "sfx_refilling_oxygen");
                            }
                        }
                    }

                }
                else {
                    gui_system->hide_oxygen_tank_gui();
                }
                float current_oxygen = GM.get_current_oxygen_level();
                if (current_oxygen > last_oxygen_level) {
                    bgm_oxygen_increasing = true;
                }
                else if (current_oxygen < last_oxygen_level) {
                    bgm_oxygen_increasing = false;
                }
                last_oxygen_level = current_oxygen;

                int current_scene = GM.get_current_scene();
                if (current_scene == 1 || current_scene == 2) {
                    ADM.update_bgm_layering(current_scene, current_oxygen, bgm_oxygen_increasing);
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

#if 1
    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
        // Constants for collision response
        const float MIN_PENETRATION = 0.001f; // Minimum penetration to respond to
        const float TOP_POSITION_CORRECTION = 2.f; 
        const float POSITION_CORRECTION = 1.0f; // Increased from 0.8f for more immediate correction
        const float CORRECTION_FACTOR = 0.15f;

        for (const auto& collision : collisions) {
            EntityID entity1 = collision.entity1;
            //EntityID entity2 = collision.entity2;

            auto& transform1 = ECSM.get_component<Transform2D>(entity1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(entity1);
            auto& physics1 = ECSM.get_component<Physics_Component>(entity1);

            // Skip if entity is static
            if (physics1.get_is_static()) continue;

            // Get collision normal based on collision side
            Vec2D normal(0.0f, 0.0f);
            switch (collision.side) {
            case CollisionSide::LEFT:   normal = Vec2D(1.0f, 0.0f); break;
            case CollisionSide::RIGHT:  normal = Vec2D(-1.0f, 0.0f); break;
            case CollisionSide::TOP:    normal = Vec2D(0.0f, -1.0f); break;
            case CollisionSide::BOTTOM: normal = Vec2D(0.0f, 1.0f); break;
            default: continue;
            }


            //for bottom collision resolve

            // Calculate relative velocity
            Vec2D relative_velocity = velocity1.velocity;
            float restitution = (collision.side == CollisionSide::BOTTOM) ? 0.0f : 0.05f;

            float impulse_scalar = -(1.0f + restitution) * dot_product_vec2d(relative_velocity, normal);
            Vec2D impulse = normal * impulse_scalar;
            velocity1.velocity += impulse * physics1.get_inv_mass();
            Vec2D correction(0.0f, 0.0f);

            if (collision.overlap.y > MIN_PENETRATION) {
                float correction_magnitude = (collision.overlap.y - MIN_PENETRATION) * CORRECTION_FACTOR;
                correction = normal * correction_magnitude;
            }

            // Handle specific collision sides
            if (collision.side == CollisionSide::BOTTOM) {
                // Ground collision - completely stop vertical movement
                correction.x = 0.0f;
                physics1.set_is_grounded(true);
                physics1.set_has_jumped(false);
                physics1.set_gravity(Vec2D(0.0f, 0.0f));

                //zero out very small vertical velocity
                if (std::abs(velocity1.velocity.y) < 0.1f) {
                    velocity1.velocity.y = 0.0f;
                }

                //position correction 
                transform1.position += correction;

                Vec2D acc_force = physics1.get_accumulated_force();
                acc_force.y = 0.0f;
                physics1.set_accumulated_force(acc_force);

            }
            else if (collision.side == CollisionSide::TOP) {
                // Ceiling collision - immediately stop upward movement and forces

                // Immediate position correction for ceiling
                if (collision.overlap.y > MIN_PENETRATION) {
                    float strong_correction = collision.overlap.y * TOP_POSITION_CORRECTION; 
                    transform1.position.y -= strong_correction;

                    //add additional downward impluse
                    velocity1.velocity.y -= 100.f; 
                }


                velocity1.velocity.y = 0.0f;
                physics1.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
                // Cancel all upward forces and jumping state
                Vec2D acc_force = physics1.get_accumulated_force();
                acc_force.y = 0.0f;  // Zero out vertical force
                physics1.set_accumulated_force(acc_force);
                physics1.set_has_jumped(false);  // Reset jump state
                physics1.reset_jump_request();    // Reset any pending jump request

                // Reset jump-related forces this might be why there is a lag on the jump
               // physics1.force_helper.deactivate_force(JUMP_UP);
            }
            else {
                // Side collisions (LEFT/RIGHT)
                velocity1.velocity.x = 0.0f;

                // Position correction for walls
                if (collision.overlap.x > MIN_PENETRATION) {
                    float horizontal_correction = collision.overlap.x * POSITION_CORRECTION;
                    if (collision.side == CollisionSide::LEFT) {
                        transform1.position.x += horizontal_correction;
                    }
                    else {
                        transform1.position.x -= horizontal_correction;
                    }
                }

                // Cancel horizontal forces in collision direction
                Vec2D acc_force = physics1.get_accumulated_force();
                if (collision.side == CollisionSide::LEFT && acc_force.x < 0) {
                    acc_force.x = 0.0f;
                }
                else if (collision.side == CollisionSide::RIGHT && acc_force.x > 0) {
                    acc_force.x = 0.0f;
                }
                physics1.set_accumulated_force(acc_force);
            }

            // Update previous position to match corrected position
            transform1.prev_position = transform1.position;
        }
    }

#endif


    void Collision_System::update(float delta_time) {
        // If we're in cooldown, decrease the timer
        if (current_cooldown > 0.0f) {
            current_cooldown -= delta_time;
            return;  // Don't process any collisions during cooldown
        }

        // Handle grace period timer
        if (grace_timer > 0.0f) {
            grace_timer -= delta_time;
            return; // Skip all collision checks during grace period
        }

        std::vector<CollisionPair> collisions;
        Boundary_Check(); 

        // If we're in the main menu scene (scene 0)
        if (GM.get_current_scene() == 0) {
            check_main_menu_button_collision(delta_time);
            return;  // Skip other collision checks for main menu
        }
        else if (GM.get_current_scene() == 3) { // Credits scene
            check_credits_back_button_collision(delta_time);
            return;  // Skip other collision checks for credits scene
        }
        else if (GM.get_current_scene() == 4) { // win screen scene
            check_win_screen_button_collision(delta_time);
            return;  // Skip other collision checks for credits scene
        }


        //cause exception thrown when player died in lava and main menu button is pressed
        /*if (GM.get_current_scene() == 2) {
            if (is_player_dead) {
                is_player_dead = GM.get_player_dead_state();
            }
        }*/
       
        
        collision_check_collide(collisions, delta_time); // Check for collisions and fill the collision list

        Detect_Obsidian_Bottom(delta_time);

        Colliside_Oxygen_Mineral(delta_time);

 
        resolve_collision_event(collisions);

        //player_interact_lava(delta_time);
       // player_interact_lava_test(delta_time);
      
        //Detect_Obsidian_Bottom(delta_time);

        //EntityID wormhole = ECSM.find_entity_by_name("spritesheet_map");
        //std::cout << "this is wormhole entity " << wormhole << "\n";
    
    }

    bool Collision_System::isInterseptBox(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
    {


        return (mouseX > (box_x - width / 2.0f) && mouseX < (box_x + width / 2.0f) &&
            mouseY >(box_y - height / 2.0f) && mouseY < (box_y + height / 2.0f));

    }

    bool Collision_System::is_transitioning = false;

    //Static variable to hold timer for clicking sound for quit button
    static float timer = 0.0f;
    static bool stop_playing = false; 

    void Collision_System::check_main_menu_button_collision(float delta_time) {
        (void)delta_time;  // Mark as intentionally unused

        if (current_cooldown > 0.0f) {
            return;  // Still in cooldown
        }

        // Reset transition flag at start of frame
        is_transitioning = false;

        // Return early if we're transitioning
        if (is_transitioning) return;

        if (GM.is_transitioning()) return;

        // Get mouse position in world coordinates
        Vec2D world_mouse_pos = ESS.Get_World_MousePos();

        //Ensure world_mouse_pos is in terms of viewport in level editor
        if (level_editor_mode) {
            world_mouse_pos.x = IMGUIM.imgui_mouse_pos().x;
            world_mouse_pos.y = IMGUIM.imgui_mouse_pos().y;
        }

        for (EntityID entity_id : get_entities()) {
            auto* entity = ECSM.get_entity(entity_id);
            if (!entity) continue;

            std::string entity_name = entity->get_name();

            // Only check for main menu buttons
            if (entity_name != "play_button" &&
                entity_name != "setting_button" &&
                entity_name != "credit_button" &&
                entity_name != "quit_button") continue;

            if (!ECSM.has_component<Transform2D>(entity_id) ||
                !ECSM.has_component<Graphics_Component>(entity_id) ||
                !ECSM.has_component<Audio_Component>(entity_id)) continue;

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& audio = ECSM.get_component<Audio_Component>(entity_id);

            // Check if mouse is hovering over the button
            bool is_hovered = ESS.Mouse_Over_AABB(
                transform.position.x,
                transform.position.y,
                transform.scale.x,
                transform.scale.y,
                world_mouse_pos.x,
                world_mouse_pos.y
            );

            // Define the base texture name for each button
            std::string base_texture;
            std::string hover_sound = "button_hover";
            std::string main_menu_sound = "main_menu";

            if (entity_name == "play_button") {
                base_texture = "Main_Menu_Play_Batch_14";
            }
            else if (entity_name == "credit_button") {
                base_texture = "Main_Menu_Credits_Batch_14";
            }
            else if (entity_name == "quit_button") {
                base_texture = "Main_Menu_Quit_Batch_14";
            }
            else if (entity_name == "setting_button") {
                base_texture = "Settings_Batch_12";
            }
           
            //Update button batch textures in the level editor
            auto& buttons_and_associated_batches = IMGUIM.return_buttons_and_batches();
            for (auto& base_textures : buttons_and_associated_batches) {
                if (entity_name == base_textures.first) {
                    base_texture = base_textures.second;
                }
            }

            if (is_hovered) {
                if (!button_hover_states[entity_name]) {
                    // Play the hover sound once when hovering
                    ADM.play_now(entity_id, hover_sound, audio);
                    button_hover_states[entity_name] = true;  // Prevent playing repeatedly
                }

                if (GM.is_mouse_left_released()) {
                    if (main_menu_sound_playing[entity_name] == false) {
                        // Play the main menu sound if it's not already playing
                        ADM.play_now(entity_id, main_menu_sound, audio);
                        main_menu_sound_playing[entity_name] = true;  // Mark sound as playing
                    }

                    // Set pressed state texture
                    graphics.texture_name = base_texture + "_PRESSED";

                    // Scene Switching Logic
                    if (entity_name == "play_button") {
                        LM.write_log("Play button held - starting fade transition to tutorial");

                        // Find GUI System to start the fade transition
                        for (auto& system : ECSM.get_systems()) {
                            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                                // Start fade transition to tutorial
                                gui_system->start_screen_fade(true, "tutorial.scn", 5);
                                break;
                            }
                        }

                        // Mark as transitioning (prevents multiple clicks)
                        is_transitioning = true;
                        current_cooldown = transition_cooldown;
                        return;
                    }
                    else if (entity_name == "setting_button") {
                        LM.write_log("Setting button held - attempting scene transition");

                        // Clear dynamic entities first
                        bool found_movement_system = false;
                        for (auto& system : ECSM.get_systems()) {
                            if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                                movement_system->clear_dynamic_entities();
                                found_movement_system = true;
                                LM.write_log("Found and cleared Movement System");
                                break;
                            }
                        }
                        if (!found_movement_system) {
                            LM.write_log("Warning: Movement System not found");
                        }

                        const std::string SCENES = "Scenes";
                        std::string scene_file = "setting.scn";
                        std::string scene_path = ASM.get_full_path(SCENES, scene_file);

                        if (SM.load_scene(scene_path.c_str())) {
                            LM.write_log("Credits scene loaded successfully");

                            // Reset camera position - add this section
                            auto& camera = GFXM.get_camera();
                            camera.pos_x = DEFAULT_CAMERA_POS_X;
                            camera.pos_y = DEFAULT_CAMERA_POS_Y;

                            // Stop all currently playing audio
                            //ADM.stop_mastergroup();

                            // Update current scene and IMGUI
                            GM.set_current_scene(6);
                            IMGUIM.set_current_file_shown(scene_file);
                            is_transitioning = true;
                            return;
                        }
                        else {
                            LM.write_log("Failed to load setting scene: %s", scene_path.c_str());
                        }
                    }
                    else if (entity_name == "credit_button") {
                        LM.write_log("Credits button held - starting fade transition to credits");

                        // Find GUI System to start the fade transition
                        for (auto& system : ECSM.get_systems()) {
                            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                                // Start fade transition to credits
                                gui_system->start_screen_fade(true, "credit.scn", 3);
                                break;
                            }
                        }

                        // Mark as transitioning (prevents multiple clicks)
                        is_transitioning = true;
                        current_cooldown = transition_cooldown;
                        return;
                    }
                    else if (entity_name == "quit_button") {
                        
                        is_transitioning = true;
                        current_cooldown = transition_cooldown;
                        stop_playing = true;

                    }
                }
                else {
                    main_menu_sound_playing[entity_name] = false; //reset
                    // Set highlighted state when just hovering
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }
            }
            else {
                // Reset to normal state texture
                graphics.texture_name = base_texture + "_NORMAL";
                button_hover_states[entity_name] = false; //reset
                main_menu_sound_playing[entity_name] = false;
            }
        }

        if (stop_playing) {

            //Time per frame
            float delta_time = FPSM.get_delta_time();

            //Progress in timer
            timer += delta_time;

            //Switching direction
            if (timer >= 0.03f) {

                LM.write_log("Quit button pressed - ending game");
                GM.set_game_over(true);

            }
        }
    }

    void Collision_System::check_credits_back_button_collision(float delta_time) {
        (void)delta_time;  // Mark as intentionally unused

        if (current_cooldown > 0.0f) {
            return;  // Still in cooldown
        }

        // Reset transition flag at start of frame
        is_transitioning = false;

        // Return early if we're transitioning
        if (is_transitioning) return;

        Vec2D world_mouse_pos = ESS.Get_World_MousePos();

        //Ensure world_mouse_pos is in terms of viewport in level editor
        if (level_editor_mode) {
            world_mouse_pos.x = IMGUIM.imgui_mouse_pos().x;
            world_mouse_pos.y = IMGUIM.imgui_mouse_pos().y;
        }

        for (EntityID entity_id : get_entities()) {
            auto* entity = ECSM.get_entity(entity_id);
            if (!entity) continue;

            std::string entity_name = entity->get_name();

            if (entity_name != "back_button") continue;

            if (!ECSM.has_component<Transform2D>(entity_id) ||
                !ECSM.has_component<Graphics_Component>(entity_id) ||
                !ECSM.has_component<Audio_Component>(entity_id)) continue;

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& audio = ECSM.get_component<Audio_Component>(entity_id);

            bool is_hovered = ESS.Mouse_Over_AABB(
                transform.position.x,
                transform.position.y,
                transform.scale.x,
                transform.scale.y,
                world_mouse_pos.x,
                world_mouse_pos.y
            );

            std::string base_texture = "Back_Batch_14";
            std::string hover_sound = "button_hover";
            std::string click_sound = "main_menu";

            //Update button batch textures in the level editor
            auto& buttons_and_associated_batches = IMGUIM.return_buttons_and_batches();
            for (auto& base_textures : buttons_and_associated_batches) {
                if (entity_name == base_textures.first) {
                    base_texture = base_textures.second;
                }
            }

            if (is_hovered) {
                if (!button_hover_states[entity_name]) {
                    // Play hover sound
                    ADM.play_now(entity_id, "button_hover", audio);
                    //audio.set_isactive("button_hover", true);
                    //audio.increase_playcount("button_hover");
                    button_hover_states[entity_name] = true;  // Prevent playing repeatedly
                }
                if (GM.is_mouse_left_released()) {
                    graphics.texture_name = base_texture + "_PRESSED";
                    ADM.play_now(entity_id, click_sound, audio);
                    //audio.increase_playcount(click_sound);
                    LM.write_log("Back button held - returning to main menu");

                    // Clear dynamic entities first
                    bool found_movement_system = false;
                    for (auto& system : ECSM.get_systems()) {
                        if (auto* movement_system = dynamic_cast<Movement_System*>(system.get())) {
                            movement_system->clear_dynamic_entities();
                            found_movement_system = true;
                            break;
                        }
                    }
                    if (!found_movement_system) {
                        LM.write_log("Warning: Movement System not found");
                    }

                    const std::string SCENES = "Scenes";
                    std::string scene_file = "main_menu.scn";
                    std::string scene_path = ASM.get_full_path(SCENES, scene_file);

                    if (SM.load_scene(scene_path.c_str())) {
                        LM.write_log("Main menu scene loaded successfully");

                        // Reset camera position
                        auto& camera = GFXM.get_camera();
                        camera.pos_x = DEFAULT_CAMERA_POS_X;
                        camera.pos_y = DEFAULT_CAMERA_POS_Y;

                        // Stop all currently playing audio
                        //ADM.stop_mastergroup();

                        // Update current scene and IMGUI
                        GM.set_current_scene(0);
                        IMGUIM.set_current_file_shown(scene_file);

                        // Set the transitioning to false for Game Manager as there is no fadeout here.
						GM.set_transitioning(false);

                        current_cooldown = transition_cooldown;  // Set the cooldown timer
                        is_transitioning = true;
                        return;
                    }
                    else {
                        LM.write_log("Failed to load main menu scene: %s", scene_path.c_str());
                    }
                }
                else {
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }
            }
            else {
                graphics.texture_name = base_texture + "_NORMAL";
                button_hover_states[entity_name] = false;
            }
        }
    }

    void Collision_System::check_win_screen_button_collision(float delta_time) {
        if (current_cooldown > 0.0f) {
            current_cooldown -= delta_time;
            return;  // Still in cooldown
        }

        // Reset transition flag at start of frame
        is_transitioning = false;

        // Return early if we're transitioning and ship is launching
        if (is_transitioning || ship_launching) return;

        Vec2D world_mouse_pos = ESS.Get_World_MousePos();

        //Ensure world_mouse_pos is in terms of viewport in level editor
        if (level_editor_mode) {
            world_mouse_pos.x = IMGUIM.imgui_mouse_pos().x;
            world_mouse_pos.y = IMGUIM.imgui_mouse_pos().y;
        }

        for (EntityID entity_id : get_entities()) {
            auto* entity = ECSM.get_entity(entity_id);
            if (!entity) continue;

            std::string entity_name = entity->get_name();

            // Only process the restart and main menu buttons
            if (entity_name != "restart_button" && entity_name != "main_menu_button") continue;

            if (!ECSM.has_component<Transform2D>(entity_id) ||
                !ECSM.has_component<Graphics_Component>(entity_id) ||
                !ECSM.has_component<Audio_Component>(entity_id)) continue;

            auto& transform = ECSM.get_component<Transform2D>(entity_id);
            auto& graphics = ECSM.get_component<Graphics_Component>(entity_id);
            auto& audio = ECSM.get_component<Audio_Component>(entity_id);

            bool is_hovered = ESS.Mouse_Over_AABB(
                transform.position.x,
                transform.position.y,
                transform.scale.x,
                transform.scale.y,
                world_mouse_pos.x,
                world_mouse_pos.y
            );

            // Base texture name
            std::string base_texture;
            if (entity_name == "restart_button") {
                base_texture = "Restart_Batch_14";
            }
            else if (entity_name == "main_menu_button") {
                base_texture = "Main_Menu_Batch_14";
            }
            std::string hover_sound = "button_hover";
            std::string click_sound = "main_menu";

            //Update button batch textures in the level editor
            auto& buttons_and_associated_batches = IMGUIM.return_buttons_and_batches();
            for (auto& base_textures : buttons_and_associated_batches) {
                if (entity_name == base_textures.first) {
                    base_texture = base_textures.second;
                }
            }

            if (is_hovered) {
                if (!button_hover_states[entity_name]) {
                    // Play hover sound
                    ADM.play_now(entity_id, hover_sound, audio);
                    button_hover_states[entity_name] = true;  // Prevent playing repeatedly
                }

                if (IM.is_mouse_button_held(GLFW_MOUSE_BUTTON_LEFT)) {
                    graphics.texture_name = base_texture + "_PRESSED";
                    ADM.play_now(entity_id, click_sound, audio);
                    // Handle button click logic
                    if (entity_name == "restart_button") {
                        LM.write_log("Win screen - Restart button pressed - starting fade transition to scene 2");
                        
                        // Find GUI System to start the fade transition
                        for (auto& system : ECSM.get_systems()) {
                            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                                // Start fade transition to scene 2
                                gui_system->start_screen_fade(true, "scene2.scn", 2);
                                break;
                            }
                        }

                        // Mark as transitioning to prevent multiple clicks
                        is_transitioning = true;
                        return;
                    }
                    else { // main_menu_button
                        LM.write_log("Win screen - Main Menu button pressed - starting fade transition to main menu");
                        
                        // Find GUI System to start the fade transition
                        for (auto& system : ECSM.get_systems()) {
                            if (auto* gui_system = dynamic_cast<GUI_System*>(system.get())) {
                                // Start fade transition to main menu
                                gui_system->start_screen_fade(true, "main_menu.scn", 0);
                                break;
                            }
                        }

                        // Mark as transitioning to prevent multiple clicks
                        is_transitioning = true;
                        current_cooldown = transition_cooldown;
                        grace_timer = post_transition_grace_period; // Set grace period
                        return;
                    }
                }
                else {
                    graphics.texture_name = base_texture + "_HIGHLIGHTED";
                }
            }
            else {
                graphics.texture_name = base_texture + "_NORMAL";
                button_hover_states[entity_name] = false;
            }
        }
    }

#if 0
    bool Collision_System::player_interact_lava_test(float delta_time)
    {

        EntityID playerID = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
        EntityID lava_pool_ID = ECSM.find_entity_by_name("lava_pool");

        if (playerID == INVALID_ENTITY_ID || lava_pool_ID == INVALID_ENTITY_ID) {
            return false; // Entities not found
        }



        auto& player_transform = ECSM.get_component<Transform2D>(playerID); // get the position of the player 
        auto& player_collision = ECSM.get_component<Collision_Component>(playerID);
        //auto& player_physic = ECSM.get_component<Physics_Component>(playerID);
        auto& player_velocity = ECSM.get_component<Velocity_Component>(playerID);

        auto& lava_collision = ECSM.get_component<Collision_Component>(lava_pool_ID);
        auto& lava_transform = ECSM.get_component<Transform2D>(lava_pool_ID); // get the position of the player 
        auto& lava_velocity = ECSM.get_component<Velocity_Component>(lava_pool_ID); // get the position of the player 

        float collisions = delta_time;
        // AABB for player
        AABB aabb_player = AABB::from_transform(player_transform, player_collision);
        AABB aabb_lava = AABB::from_transform(lava_transform, lava_collision);

        if (collision_intersection_rect_rect(aabb_player, player_velocity.velocity, aabb_lava, lava_velocity.velocity, collisions, delta_time))
        {
            std::cout << "yes!!!!\n";
            return true;
        }
        else {
            std::cout << "noo!!!\n";
        }
        return false;
    }

#endif

#if 0
    bool Collision_System::player_interact_lava_test(float delta_time)
    {
        EntityID playerID = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
        EntityID lava_pool_ID = ECSM.find_entity_by_name("lava_pool");

        if (playerID == INVALID_ENTITY_ID || lava_pool_ID == INVALID_ENTITY_ID) {
            return false; // Entities not found
        }

        auto& player_transform = ECSM.get_component<Transform2D>(playerID); // get the position of the player
        auto& player_collision = ECSM.get_component<Collision_Component>(playerID);
        auto& player_velocity = ECSM.get_component<Velocity_Component>(playerID);

        auto& lava_collision = ECSM.get_component<Collision_Component>(lava_pool_ID);
        auto& lava_transform = ECSM.get_component<Transform2D>(lava_pool_ID);
        auto& lava_velocity = ECSM.get_component<Velocity_Component>(lava_pool_ID);

        float collisions = delta_time;
        // AABB for player
        AABB aabb_player = AABB::from_transform(player_transform, player_collision);
        AABB aabb_lava = AABB::from_transform(lava_transform, lava_collision);

        if (collision_intersection_rect_rect(aabb_player, player_velocity.velocity, aabb_lava, lava_velocity.velocity, collisions, delta_time)) {
            std::cout << "Player interacts with lava! (with delta_time)\n";
            return true;
        }
        else {
            std::cout << "No interaction with lava. (with delta_time)\n";
        }

        return false;
    }
#endif


#if 0
    void Collision_System::player_interact_lava(float delta_time)
    {
        
        if (GM.get_current_scene() != 2) {
            if (is_player_dead) {
                is_player_dead = GM.get_player_dead_state();
            }
            return;
        }

        if (GM.is_restarting()) {
            if (is_player_dead && cooldown_restart <= 0.0f) {
                is_player_dead = false;
                GM.set_restarting(false);
                cooldown_restart = COOLDOWN_TIMER_FOR_RESTART;
            }
            else {
                cooldown_restart -= FPSM.get_delta_time();
            }
            return;
        }

        if (is_player_dead) {
            return;
        }

        EntityID playerID = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
        EntityID lava_pool_ID = ECSM.find_entity_by_name("lava_pool");

     
        auto& player_transform = ECSM.get_component<Transform2D>(playerID); // get the position of the player 
        auto& player_collision = ECSM.get_component<Collision_Component>(playerID);
        //auto& player_physic = ECSM.get_component<Physics_Component>(playerID);
        auto& player_velocity = ECSM.get_component<Velocity_Component>(playerID);

        auto& lava_collision = ECSM.get_component<Collision_Component>(lava_pool_ID);
        auto& lava_transform = ECSM.get_component<Transform2D>(lava_pool_ID); // get the position of the player 
        auto& lava_velocity = ECSM.get_component<Velocity_Component>(lava_pool_ID); // get the position of the player 

        float collisions = delta_time;
        // AABB for player
        AABB aabb_player = AABB::from_transform(player_transform, player_collision);
        AABB aabb_lava = AABB::from_transform(lava_transform, lava_collision);

        //std::cout << "lava pos x:" << lava_transform.position.x << "lava pos y: " << lava_transform.position.y << "\n";
        //std::cout << "lava height: " << lava_collision.height << " lava width: " << lava_collision.width << " collidable: " << lava_collision.collidable << "\n";
        //bool is_player_dead = false;
        if (collision_intersection_rect_rect(aabb_player, player_velocity.velocity, aabb_lava, lava_velocity.velocity, collisions, delta_time))
        {
           std::cout << "test is interact\n";
            is_player_dead = true;
            GM.set_player_dead_state(true);
        }

        // Check if player is dead to reset the scene
        if (is_player_dead == true) {
            // Stop all audio first
            //ADM.stop_mastergroup();

            ADM.stop_groups(GroupType::TYPE_BGM);
            ADM.stop_groups(GroupType::TYPE_SFX);

            //reset panic
            GM.reset_panic();

            // Find GUI System and show game over screen
            for (auto& systems_gui : ECSM.get_systems()) {
                if (auto* gui_system = dynamic_cast<GUI_System*>(systems_gui.get())) {
                    // First reset all GUI states
                    gui_system->reset_all_game_state();

                    // Then show the game over screen
                    gui_system->show_game_over_menu();

                    LM.write_log("Game over screen displayed - player killed by Lava");
                    break;
                }
            }
        }

    }

#endif 

#if 1
    void Collision_System::Detect_Obsidian_Bottom(float delta_time)
    {
        // Only for game play scene
        if (GM.get_current_scene() != 2) {
            return;
        }
        
        // Find entities
        EntityID obsidian_entity = ECSM.find_entity_by_name("obsidian_bottom");
        EntityID player_ID = ECSM.find_entity_by_name(DEFAULT_PLAYER_NAME);
        if (obsidian_entity == static_cast<EntityID>(-1) || player_ID == static_cast<EntityID>(-1)) {
            return;
        }

        // Get components
        auto& player_transform = ECSM.get_component<Transform2D>(player_ID);
        auto& player_collision = ECSM.get_component<Collision_Component>(player_ID);
        auto& player_velocity = ECSM.get_component<Velocity_Component>(player_ID);
        auto& player_physic = ECSM.get_component<Physics_Component>(player_ID);
        auto& obsidian_transform = ECSM.get_component<Transform2D>(obsidian_entity);
        auto& obsidian_collision = ECSM.get_component<Collision_Component>(obsidian_entity);

        // Create AABBs
        AABB aabb_player = AABB::from_transform(player_transform, player_collision);
        AABB aabb_obsidian = AABB::from_transform(obsidian_transform, obsidian_collision);

        // Check for collision
        float collision_time = delta_time;
        if (collision_intersection_rect_rect(aabb_player, player_velocity.velocity, aabb_obsidian, Vec2D(0.0f, 0.0f), collision_time, delta_time)) {
            CollisionSide side = compute_collision_side(aabb_player, aabb_obsidian);

            // Handle bottom collision with obsidian
            if (side == CollisionSide::BOTTOM) {
                // Adjust player position
                float overlap = aabb_player.min.y - aabb_obsidian.max.y;
                player_transform.position.y -= overlap;

                // Reset jump state to allow jumping again
                player_physic.set_is_grounded(true);
                player_physic.set_has_jumped(false);
                player_physic.set_gravity(Vec2D(0.0f, 0.0f));

            }
        }
    }
#endif
}


