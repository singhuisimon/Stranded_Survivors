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



namespace lof {

    AABB::AABB(const Vec2D& min, const Vec2D& max)
        : min(min), max(max) {}

    AABB AABB::from_transform(const Transform2D& transform, const Collision_Component& collision) {
        Vec2D min;
        Vec2D max;

        min.x = transform.prev_position.x - (collision.width / 2.0f);
        min.y = transform.prev_position.y - (collision.height / 2.0f);

        max.x = transform.prev_position.x + (collision.width / 2.0f);
        max.y = transform.prev_position.y + (collision.height / 2.0f);

        // Debug print to verify min and max values
        //std::cout << "AABB Min in from_transform function in collision system: (" << min.x << ", " << min.y << "), Max: (" << max.x << ", " << max.y << ")\n";

        return AABB(min, max);
    }

    Collision_System::Collision_System() {
        // Set the required components for this system
        signature.set(ECSM.get_component_id<Transform2D>());
        signature.set(ECSM.get_component_id<Collision_Component>());
        signature.set(ECSM.get_component_id<Physics_Component>());
        signature.set(ECSM.get_component_id<Velocity_Component>());


        LM.write_log("Collision_System initialized with signature requiring Transform2D, Collision_Component, Physics_Component, and Velocity_Component.");
    }

    std::string Collision_System::get_type() const {
        return "Collision_System";
    }

 
    void Collision_System::collision_check_collide(std::vector<CollisionPair>& collisions, float delta_time) {
        // Iterate over entities matching the system's signature
        const auto& entities = get_entities();
        //std::cout << "Total entities: " << entities.size() << "\n";

        for (auto it1 = entities.begin(); it1 != entities.end(); ++it1) {
            EntityID entity_ID1 = *it1;
            //std::cout << *it1 << "\n";

            auto& physic1 = ECSM.get_component<Physics_Component>(entity_ID1);
          

            // Skip if entity is static
            if (physic1.is_static)
                continue;
           
                
            auto& transform1 = ECSM.get_component<Transform2D>(entity_ID1);
            auto& collision1 = ECSM.get_component<Collision_Component>(entity_ID1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(entity_ID1);

            // Create AABB for object 1
            AABB aabb1 = AABB::from_transform(transform1, collision1);
            //std::cout << "Entity " << entity_ID1 << " is_static: " << physic1.is_static << "\n";
            

            bool is_grounded = false; // Track if entity is grounded
            // Check for collisions with other entities
            auto it2 = std::next(it1); // Start from the next entity
            //std::cout << "Starting inner loop for Entity " << entity_ID1 << "\n";

            // Check for collisions with other entities
            for (auto it2 = entities.begin(); it2 != entities.end(); ++it2) {
                EntityID entity_ID2 = *it2;
                //std::cout << "entity 1 is :" << entity_ID1 << "\n";
                if (entity_ID1 == entity_ID2)
                {
                    continue;
                }


                auto& physic2 = ECSM.get_component<Physics_Component>(entity_ID2);

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
                        physic1.gravity.y = 0.0f;
                    }
                  

                    // Store collision pair and overlap information
                    collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2), side });
                    //std::cout << "Entity " << entity_ID1 << " collides with Entity " << entity_ID2 << " on side: " << static_cast<int>(side) << "\n";

                    //std::cout << "Entity " << entity_ID1 << " Position: (" << transform1.position.x << ", " << transform1.position.y << ")\n";
                    //std::cout << "Entity " << entity_ID2 << " Position: (" << transform2.position.x << ", " << transform2.position.y << ")\n";
                    //std::cout << "State of is grouded for player: " << physic1.is_grounded << "\n";
                    
                    
                }
            }
            physic1.is_grounded = is_grounded;
            if (!is_grounded) {
                physic1.gravity.y = DEFAULT_GRAVITY; // Reset gravity if not grounded
            }
        }
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
        //std::cout << "Overlap X: " << overlap_x << ", Overlap Y: " << overlap_y << std::endl;


        return Vec2D(overlap_x, overlap_y);
    }

    std::string collisionSideToString(CollisionSide side) {
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

        //// Determine the side of the collision
        //if (overlapX >= 0 && overlapY >= 0) {
        //    if (overlapX < overlapY) {
        //        return (dx > 0) ? CollisionSide::LEFT : CollisionSide::RIGHT;
        //       
        //    }
        //    else {
        //        return (dy > 0) ? CollisionSide::TOP : CollisionSide::BOTTOM;

        //       
        //    }
        //}

      // Determine the side of the collision
        CollisionSide side = CollisionSide::NONE;

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


    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
        for (const auto& collision : collisions) {
            EntityID entityA = collision.entity1;
            EntityID entityB = collision.entity2;

            auto& physicsA = ECSM.get_component<Physics_Component>(entityA);
            auto& transformA = ECSM.get_component<Transform2D>(entityA);
            auto& velocityA = ECSM.get_component<Velocity_Component>(entityA);

            auto& physicsB = ECSM.get_component<Physics_Component>(entityB);
            auto& transformB = ECSM.get_component<Transform2D>(entityB);
            auto& velocityB = ECSM.get_component<Velocity_Component>(entityB);


            // Only stop the player (entityA) if it is a dynamic object colliding with a static object (entityB)
            //if (!physicsA.is_static && !physicsB.is_static) {
                // Determine the side of collision for response
                CollisionSide collisionSide = collision.side;
                Vec2D overlap = collision.overlap;


                // Adjust position and velocity based on the collision side and overlap
                switch (collisionSide) {
                case CollisionSide::LEFT:
                    //velocityA.velocity.x = std::max(0.0f, velocityA.velocity.x);
                    velocityA.velocity.x = 0.0f;
                    transformA.position.x += overlap.x;
                    //std::cout << "This is the position in resolve function for left side " << transformA.position.x << "\n";
                    break;

                case CollisionSide::RIGHT:
                    //velocityA.velocity.x = std::min(0.0f, velocityA.velocity.x);
                    velocityA.velocity.x = 0.0f;
                    transformA.position.x -= overlap.x;
                    std::cout << "This is the position in resolve function for right side " << transformA.position.x << "\n";
                    break;

                case CollisionSide::TOP:
                    //velocityA.velocity.y = std::max(0.0f, velocityA.velocity.y);
                    velocityA.velocity.y = 0.0f;
                    //transformA.position.y = transformB.position.y + transformB.scale.y * 0.5 + transformA.scale.y * 0.5;
                    transformA.position.y -= overlap.y;
                    //std::cout << "this is the scale for platformB in resolve solution" << transformB.scale.y << "\n";
                    //std::cout << "This is the position in resolve function for top side " << transformA.position.y << "\n";
                    break;

                case CollisionSide::BOTTOM:
                    //velocityA.velocity.y = std::min(0.0f, velocityA.velocity.y);
                    velocityA.velocity.y = 0.0f;
                    transformA.position.y += overlap.y;
                    //std::cout << "This is the position in resolve function for bottom side " << transformA.position.y << "\n";
                    break;

                case CollisionSide::NONE:
                    velocityA.velocity.x = 0.0f;
                    velocityA.velocity.y = 0.0f;
                    break;

                default:
                    break;
                }
            }
       // }


    
    }

    void Collision_System::update(float delta_time) {
        std::vector<CollisionPair> collisions;
       // std::cout << "---------------------------this is check collide in collision syystem----------------------------------------\n";
        collision_check_collide(collisions, delta_time); // Check for collisions and fill the collision list
        //std::cout << "---------------------------this is end of check collide in collision syystem----------------------------------------\n";
        resolve_collision_event(collisions);
    }


} // namespace lof

#if 0

   /* std::cout << "entity 2 is :" << entity_ID2 << "\n";
                std::cout << "Velocity for Entity " << entity_ID1 << ": (" << velocity1.velocity.x << ", " << velocity1.velocity.y << ")\n";
                std::cout << "Velocity for Entity " << entity_ID2 << ": (" << velocity2.velocity.x << ", " << velocity2.velocity.y << ")\n";*/


//std::cout << "Checking collision between Entity " << entity_ID1 << " and Entity " << entity_ID2 << "\n";
                ////std::cout << "AABB1: Min(" << aabb1.min.x << ", " << aabb1.min.y << "), Max(" << aabb1.max.x << ", " << aabb1.max.y << ")\n";
                ////std::cout << "AABB2: Min(" << aabb2.min.x << ", " << aabb2.min.y << "), Max(" << aabb2.max.x << ", " << aabb2.max.y << ")\n";
                //std::cout << "Before Collision Check: Entity 4 Position: (" << transform1.prev_position.x << ", " << transform1.prev_position.y << ")\n";
                //std::cout << "Current Collision Check: Entity 4 Position: (" << transform1.position.x << ", " << transform1.position.y << ")\n";
                //std::cout << "Entity: " << entity_ID2 << " Before Collision Check: other entity Position: (" << " " << transform2.prev_position.x << ", " << transform2.prev_position.y << ")\n";
                //std::cout << "Entity: " << entity_ID2 << " current Collision Check: other entity Position: (" << " " << transform2.position.x << ", " << transform2.position.y << ")\n";

          /*  if (!physicsA.is_static)
            {
                velocityA.velocity.x = 0.0f;
                velocityA.velocity.y = 0.0f;
                std::cout << entityA << "is static\n";

            }*/
#endif