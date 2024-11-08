/**
 * @file Collision_Syetem.h
 * @brief Implements the declaration of collsion system.
 * @author Saw Hui Shan (100%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
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
        bool is_grounded;
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

        

    private:
        
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

    };

} // namespace lof

#endif // LOF_COLLISION_SYSTEM_H