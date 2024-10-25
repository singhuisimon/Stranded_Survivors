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
#include "../Utility/Constant.h"
#include "../Manager/ECS_Manager.h"

// Initialize Value
float OVERLAP_X = 0.0f;
float OVERLAP_Y = 0.0f;


namespace lof {

	/**
	* @brief create AABB from transform component and collision component
	* @param transform Tranform data including position, rotation, scale
	* @param collision Collision component data including width and height
	* @return AABB instance
	*/
#if 1
	AABB AABB::from_Tranform(const Transform2D& transform, const Collision_Component& collision)
	{
		Vec2D boundingRect_min;
		Vec2D boundingRect_max;


		boundingRect_min.x = transform.position.x - (collision.width / 2.0f);
		boundingRect_min.y = transform.position.y - (collision.height / 2.0f);

		boundingRect_max.x = transform.position.x + (collision.width / 2.0f);
		boundingRect_max.y = transform.position.y + (collision.height / 2.0f);



		return AABB(boundingRect_min, boundingRect_max); //return the created AABB
	}
#endif


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

    void Collision_System::update(float delta_time) {
        std::vector<CollisionPair> collisions;
        collision_check_collide(collisions, delta_time); // Check for collisions and fill the collision list
        resolve_collision_event(collisions);
    }

    void Collision_System::collision_check_collide(std::vector<CollisionPair>& collisions, float delta_time) {
        // Iterate over entities matching the system's signature
        const auto& entities = get_entities();

        for (auto it1 = entities.begin(); it1 != entities.end(); ++it1) {
            EntityID entity_ID1 = *it1;

            auto& physic1 = ECSM.get_component<Physics_Component>(entity_ID1);

            // Skip if entity is static
            if (physic1.is_static)
                continue;

            auto& transform1 = ECSM.get_component<Transform2D>(entity_ID1);
            auto& collision1 = ECSM.get_component<Collision_Component>(entity_ID1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(entity_ID1);

            // Create AABB for object 1
            AABB aabb1 = AABB::from_transform(transform1, collision1);

            // Check for collisions with other entities
            for (auto it2 = std::next(it1); it2 != entities.end(); ++it2) {
                EntityID entity_ID2 = *it2;

                auto& physic2 = ECSM.get_component<Physics_Component>(entity_ID2);

                auto& transform2 = ECSM.get_component<Transform2D>(entity_ID2);
                auto& collision2 = ECSM.get_component<Collision_Component>(entity_ID2);
                auto& velocity2 = ECSM.get_component<Velocity_Component>(entity_ID2);

                // Create AABB for object 2
                AABB aabb2 = AABB::from_transform(transform2, collision2);

                // Check for intersection between two entities
                float collision_time = delta_time;
                if (collision_intersection_rect_rect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time)) {
                    // Store collision pair and overlap information
                    collisions.push_back({ entity_ID1, entity_ID2, compute_overlap(aabb1, aabb2) });
                }
            }
        }
    }

    bool Collision_System::collision_intersection_rect_rect(const AABB& aabb1,
        const Vec2D& vel1,
        const AABB& aabb2,
        const Vec2D& vel2,
        float& firstTimeOfCollision) {
        // Check for AABB intersection
        if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x ||
            aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) {
            return false; // No intersection
        }

        // Additional collision detection logic can be implemented here if needed

        return true; // The rectangles intersect
    }

    Vec2D Collision_System::compute_overlap(const AABB& aabb1, const AABB& aabb2) {
        float overlap_x = std::min(aabb1.max.x, aabb2.max.x) - std::max(aabb1.min.x, aabb2.min.x);
        float overlap_y = std::min(aabb1.max.y, aabb2.max.y) - std::max(aabb1.min.y, aabb2.min.y);

        // Ensure overlaps are non-negative
        overlap_x = std::max(0.0f, overlap_x);
        overlap_y = std::max(0.0f, overlap_y);

        return Vec2D(overlap_x, overlap_y);
    }

    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
        for (const auto& collision : collisions) {
            auto& transform1 = ECSM.get_component<Transform2D>(collision.entity1);
            auto& velocity1 = ECSM.get_component<Velocity_Component>(collision.entity1);
            auto& physic1 = ECSM.get_component<Physics_Component>(collision.entity1);

            auto& transform2 = ECSM.get_component<Transform2D>(collision.entity2);
            auto& velocity2 = ECSM.get_component<Velocity_Component>(collision.entity2);
            auto& physic2 = ECSM.get_component<Physics_Component>(collision.entity2);

            AABB aabb1 = AABB::from_transform(transform1, ECSM.get_component<Collision_Component>(collision.entity1));
            AABB aabb2 = AABB::from_transform(transform2, ECSM.get_component<Collision_Component>(collision.entity2));

            // Resolve the collision for entity1
            resolve_collision_static_dynamic(aabb1, aabb2, transform1, velocity1.velocity, collision.overlap);

            // If entity2 is not static, resolve collision for entity2
            if (!physic2.is_static) {
                Vec2D adjusted_overlap = collision.overlap;
                // Adjust overlap direction for entity2
                resolve_collision_static_dynamic(aabb2, aabb1, transform2, velocity2.velocity, adjusted_overlap);
            }

            // Update physics states
            if (collision.overlap.y > 0) {
                physic1.is_grounded = true;
                physic1.is_jumping = false;
            }
        }
    }

    void Collision_System::resolve_collision_static_dynamic(const AABB& aabb1, const AABB& aabb2, Transform2D& transform1, Vec2D& velocity1, const Vec2D& overlap) {
        if (overlap.x < overlap.y) {
            // Resolve along x-axis
            if (aabb1.min.x < aabb2.min.x) {
                // Collision from the left
                transform1.position.x -= overlap.x;
            }
            else {
                // Collision from the right
                transform1.position.x += overlap.x;
            }
            // Zero out x-velocity
            velocity1.x = 0.0f;
        }
        else {
            // Resolve along y-axis
            if (aabb1.min.y < aabb2.min.y) {
                // Collision from below
                transform1.position.y -= overlap.y;
                // Entity is grounded
                velocity1.y = 0.0f;
            }
            else {
                // Collision from above
                transform1.position.y += overlap.y;
                velocity1.y = 0.0f;
            }
        }
    }

} // namespace lof