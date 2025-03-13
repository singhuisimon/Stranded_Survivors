/**
 * @file Collision_Syetem.h
 * @brief Implements the declaration of collsion system.
 * @author Saw Hui Shan (100%)
 * @date September 21, 2024
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once

#ifndef LOF_COLLISION_SYSTEM_H
#define LOF_COLLISION_SYSTEM_H

 // Include other necessary headers
#include "../Utility/Vector2D.h" //to get the library from vector2D.h
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"
#include "System.h"

//include standard header
#include <iostream>

namespace lof {
#define CS lof::Collision_System::get_instance()

    // Enum for vent directiono 
    enum class VentDirection {
        NONE, 
        UP,
        LEFT, 
        RIGHT
    };

    // A enum class for the possible collide side 
    enum class CollisionSide {
        NONE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };


    /**
    * @struct CollisionPair
    * @brief Represents a pair of entities that have collided, along with overlap information.
    */
    struct CollisionPair {
        EntityID entity1;
        EntityID entity2;
        Vec2D overlap;
        CollisionSide side;
        float collide_time; // to store when it collide
    };

    /**
     * @struct AABB
     * @brief Represents axis-aligned bounding boxes for collision detection.
     */
    struct AABB {
        Vec2D min; ///< Minimum point of the bounding box
        Vec2D max; ///< Maximum point of the bounding box

        /**
         * @brief Constructor to initialize minimum and maximum points of AABB.
         * @param min Minimum corner of the bounding box.
         * @param max Maximum corner of the bounding box.
         */
        AABB(const Vec2D& min, const Vec2D& max);

        /**
         * @brief Create AABB from Transform2D and Collision_Component.
         * @param transform Transform component data including position, rotation, scale.
         * @param collision Collision component data including width and height.
         * @return AABB instance.
         */
        static AABB from_transform(const Transform2D& transform, const Collision_Component& collision);
    };

    
    //struct PointLine{
    //    Vec2D center; //this will be player's center
    //    Vec2D edge; //this is the edge extending the player 

    //    PointLine(const Vec2D& center, const Vec2D& edge); 

    //   // PointLine create_Line(const Transform2D& transform, const CollisionSide side, const Collision_Component& collision);
    //};

    // extern SelectedEntityInfo g_selected_Entity_Info;
     /**
      * @class Collision_System
      * @brief Handles collision detection and resolution between entities.
      */
    class Collision_System : public System {
    public:


        /**
         * @brief Constructor for Collision_System.
         * Initializes the system's signature.
         */
        Collision_System();

        /**
         * @brief Update the collision system.
         * @param delta_time Delta time since the last update.
         */
        void update(float delta_time) override;



        /**
         * @brief Returns the type of the collision system.
         * @return String representing the type.
         */
        std::string get_type() const override;

        //int Check_Instance_Collision(float pos_x, float pos_y, float scale_x, float scale_y);

        CollisionSide compute_collision_side(const AABB& aabb1, const AABB& aabb2);

        /**
         * @brief Check for intersection between rectangles.
         * @param aabb1 First AABB.
         * @param vel1 First velocity.
         * @param aabb2 Second AABB.
         * @param vel2 Second velocity.
         * @param firstTimeOfCollision Output param to hold the time of collision.
         * @return True if rectangles intersect, false otherwise.
         */
        bool collision_intersection_rect_rect(const AABB& aabb1,
            const Vec2D& vel1,
            const AABB& aabb2,
            const Vec2D& vel2,
            float& firstTimeOfCollision,
            float delta_time);

        bool isInterseptBox(float box_x, float box_y, float width, float height, int X, int mouseY);

        static Collision_System& get_instance();

        bool has_bottom_collide_detect() const { return has_bottom_collision; }
        bool has_left_collide_detect() const { return has_left_collision; }
        bool has_right_collide_detect() const { return has_right_collision; }
        bool has_top_collide_detect() const { return has_top_collision; }


        EntityID get_bottom_collide_entity() const { return static_cast<int>(bottom_collision_entity); }
        EntityID get_left_collide_entity() const { return static_cast<int>(left_collision_entity); }
        EntityID get_right_collide_entity() const { return static_cast<int>(right_collision_entity); }
        EntityID get_top_collide_entity() const { return static_cast<int>(top_collision_entity); }

        EntityID get_detect_entities() const { return static_cast<int>(check_non_collidable_entities); }
        bool entities_detected() const { return entites_detect; }

        EntityID mineral_tank_detected() const { return static_cast<int>(mineral_tank); }
        EntityID oxygen_tank_detected() const { return static_cast<int>(oxygen_tank); }

       
        /**
        * @brief Ensure that the player does not go outside the game world boundaries horizontally.
        */
        void Boundary_Check();
      
        void reset_deposited_minerals() {
            total_deposited_minerals = 0;
        }


    private:
        static std::unique_ptr<Collision_System> instance;

        static std::once_flag once_flag;

        // for detect the rows and cols to destroy
        int frame_counter = 0;
        static bool has_bottom_collision;
        static bool has_left_collision;
        static bool has_right_collision;
        static bool has_top_collision;

        static EntityID bottom_collision_entity;
        static EntityID left_collision_entity;
        static EntityID right_collision_entity;
        static EntityID top_collision_entity;

        //int previous_value = 0;
        bool deposit = false;
        bool e_press = false;
        bool e_last_frame = false;

        static EntityID check_non_collidable_entities;
        static bool entites_detect;

        //incase want specific entity
        static EntityID mineral_tank;
        static EntityID oxygen_tank;

        static bool is_transitioning;
        float transition_cooldown = 0.5f;
        float current_cooldown = 0.0f;

        int total_deposited_minerals = 0;

        float post_transition_grace_period = 0.5f; // Grace period after transition
        float grace_timer = 0.0f; // Timer to track grace period

        std::unordered_map<std::string, bool> button_hover_states;
        std::unordered_map<std::string, bool> main_menu_sound_playing;

        // to get oxygen
        //static bool oxygen_increasing;
        //static bool collision_handled;
        //float accumulated_time = 0.0f;

        //sstd::vector<CollisionPair> collision_pairs; // Store collisions

        /**
         * @brief Compute the overlap between AABBs.
         * @param aabb1 First AABB object.
         * @param aabb2 Second AABB object.
         * @return The Vec2D value representing the overlap.
         */
        Vec2D compute_overlap(const AABB& aabb1, const AABB& aabb2);

        /**
         * @brief Check if collisions occur between entities.
         * @param collisions A reference to a vector of CollisionPair objects for their overlap information.
         * @param delta_time The time since the last update.
         */
        void collision_check_collide(std::vector<CollisionPair>& collisions, float delta_time);
        void collision_check_scene1(std::vector<CollisionPair>& collisions, float delta_time);
        void collision_check_scene2(std::vector<CollisionPair>& collisions, float delta_time);


        /*
        * @brief Checks if an entity is a vent type object by check its name 
        * @param id the EntityID to check
        * @return VentDirection enum type (UP, LEFT, RIGHT, NONE)
        */

        VentDirection get_vent_direction(EntityID id) const;

        /*
        * apply the force on the player depending on the direction. stop and play the vent audio as well
        */
        void apply_vent_force(EntityID id, VentDirection direction, bool found_next_vent, bool& is_grounded); 

        /*
        * @brief Handles collision interactions between an entity and a vent
        * @param entity The EntityID of the entity interacting with the vent
        * @param vent The EntityID of the vent being interacted with 
        * @param delta_time fixed delta_time
        * @param is_grounded Reference to flage indicating if entity is on the ground
        */
        void handle_vent_collision(EntityID entity, EntityID vent, float delta_time, bool& is_grounded);

        /**
         * @brief Resolve collisions and update the positions and velocities of involved entities.
         * @param collisions A reference to a vector of CollisionPair objects for their overlap information.
         */
        void resolve_collision_event(const std::vector<CollisionPair>& collisions);

        /**
         * @brief Resolve collision between a dynamic object and a static object.
         * @param aabb1 First AABB object.
         * @param aabb2 Second AABB object.
         * @param transform1 The Transform2D of the dynamic object.
         * @param velocity1 The velocity of the dynamic object.
         * @param overlap The overlap between AABBs along the x and y axis.
         */
         //void resolve_collision_static_dynamic(const AABB& aabb1, const AABB& aabb2, Transform2D& transform1, Vec2D& velocity1, const Vec2D& overlap);


         /**
          * @brief Get the string of which side to collide
          * @param side The side of the collide side
          * @return Return the string side that is collide
          */
        std::string collisionSideToString(CollisionSide side);

        void Colliside_Oxygen_Mineral(float delta_time);


        // == MAIN MENU BUTTON COLLISION ==
        void check_main_menu_button_collision(float delta_time);
        // == CREDITS BUTTON COLLISION ==
        void check_credits_back_button_collision(float delta_time);
        // == WIN SCREEN BUTTON COLLISION ==
        void check_win_screen_button_collision(float delta_time);
    };



} // namespace lof

#endif // LOF_COLLISION_SYSTEM_H