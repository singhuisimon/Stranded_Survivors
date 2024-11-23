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
#include "Entity_Selector_System.h"





namespace lof {
    std::unique_ptr<Collision_System> Collision_System::instance;
    std::once_flag Collision_System::once_flag;

    SelectedEntityInfo Collision_System::g_selected_Entity_Info;

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
        signature.set(ECSM.get_component_id<Transform2D>()); // simon
        signature.set(ECSM.get_component_id<Collision_Component>()); // simon
        signature.set(ECSM.get_component_id<Physics_Component>()); // simon
        signature.set(ECSM.get_component_id<Velocity_Component>()); // simon

        LM.write_log("Collision_System initialized with signature requiring Transform2D, Collision_Component, Physics_Component, and Velocity_Component."); //simon
    }

    std::string Collision_System::get_type() const {
        return "Collision_System";
    }
 
    void Collision_System::collision_check_collide(std::vector<CollisionPair>& collisions, float delta_time) {
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
                    
#if 1
                   // std::cout << "Entity " << entity_ID1 << " Position: (" << transform1.position.x << ", " << transform1.position.y << ")\n";
                    //std::cout << "Entity " << entity_ID2 << " Position: (" << transform2.position.x << ", " << transform2.position.y << ")\n";
                    //std::cout << "State of is grouded for player: " << physic1.get_is_grounded() << "\n";
#endif                
                    
                }
            }
            physic1.set_is_grounded(is_grounded);
            if (!is_grounded) {
                physic1.set_gravity(Vec2D(0.0f, DEFAULT_GRAVITY));
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

#if 1

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
#endif
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


#if 0
    void Collision_System::resolve_collision_event(const std::vector<CollisionPair>& collisions) {
        for (const auto& collision : collisions) {
            EntityID entityA = collision.entity1;
            //EntityID entityB = collision.entity2;

            auto& transformA = ECSM.get_component<Transform2D>(entityA);
            auto& velocityA = ECSM.get_component<Velocity_Component>(entityA);

            //auto& velocityB = ECSM.get_component<Velocity_Component>(entityB);


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
                    // std::cout << "This is the position in resolve function for right side " << transformA.position.x << "\n";
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

                std::cout << "Entity " << entityA << " collision side: " << static_cast<int>(collisionSide)
                    << ", velocity: (" << velocityA.velocity.x << ", " << velocityA.velocity.y
                    << "), position: (" << transformA.position.x << ", " << transformA.position.y << ")\n";



            }
       // }
    
    }
#endif


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



    void Collision_System::update(float delta_time) {
        std::vector<CollisionPair> collisions;
        // std::cout << "---------------------------this is check collide in collision syystem----------------------------------------\n";
        collision_check_collide(collisions, delta_time); // Check for collisions and fill the collision list
        //std::cout << "---------------------------this is end of check collide in collision syystem----------------------------------------\n";
        resolve_collision_event(collisions);
        //Check_Selected_Entity();

       
    
#if 0
    if (entitySelected) {
        std::cout << "Selected Entity ID: " << selectedEntityID << "\n";
        // Optionally, print other information about the selected entity, such as mouse position
        SelectedEntityInfo selectedInfo = Get_Selected_Entity_Info();
        std::cout << "Mouse Position: (" << selectedInfo.mousePos.x << ", " << selectedInfo.mousePos.y << ")\n";
    }
    else {
        std::cout << "No entity is selected.\n";
    }
#endif

    }

    bool Collision_System::isInterseptBox(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
    {

        
        return (mouseX > (box_x - width / 2.0f) && mouseX < (box_x + width / 2.0f) &&
            mouseY >(box_y - height / 2.0f) && mouseY < (box_y + height / 2.0f));
        
    }

#if 0
    void Collision_System::Check_Selected_Entity()
    {
        bool entitySelected = false;
        EntityID selectedEntityID = -1;

        for (EntityID entityID : get_entities())
        {
            auto& transform = ECSM.get_component<Transform2D>(entityID);
            auto& collision = ECSM.get_component<Collision_Component>(entityID);

            float entityX = transform.position.x;
            float entityY = transform.position.y;
            float entityWidth = collision.width;
            float entityHeight = collision.height;

            Update_Selected_Entity_Info(entityID, entityX, entityY, entityWidth, entityHeight);
            SelectedEntityInfo selectedInfo = g_selected_Entity_Info;

            if (selectedInfo.isSelected) {
                entitySelected = true;
                selectedEntityID = selectedInfo.selectedEntity;  // Store the selected entity ID
                break;  // Exit the loop early if an entity is selected 
            }

        }
    }


    Vec2D Collision_System::Get_World_MousePos()
    {
        /*double mouse_x;
        double mouse_y;*/

        if (!window)
        {
            return Vec2D(0.0f, 0.0f);
        }
 
    
        double screen_width = SM.get_scr_width();
        double screen_height = SM.get_scr_height(); //1020 /2
        //std::cout << "scene width: " << SM.get_scr_width() << " scene height: " << SM.get_scr_height() << "\n";
        // 1980 x 1020
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        mouse_x -= (screen_width / 2);
        mouse_y -= (screen_height /2);
        mouse_x = mouse_x;
        mouse_y = mouse_y;
        mouse_y = -mouse_y;

        // Get camera position
        auto& camera = GFXM.get_camera();

        // Convert screen coordinates to world coordinates by adding camera position
        double world_x = mouse_x + camera.pos_x;
        double world_y = mouse_y + camera.pos_y;
       // std::cout << "cursor position x: " << world_x << " " << "cursor position y: " << world_y << "\n";
        return Vec2D(static_cast<float>(world_x), static_cast<float>(world_y));
     
      

    }


    void Collision_System::Update_Selected_Entity_Info(EntityID entityID, float entityX, float entityY, float entityWidth, float entityHeight)
    {
       
        Vec2D mousePos = Get_World_MousePos();
        entitySelectionInfo.mousePos = mousePos;

        bool isSelected = isInterseptBox(entityX, entityY, entityWidth, entityHeight, mousePos.x, mousePos.y);
        entitySelectionInfo.isSelected = isSelected;

        if (entitySelectionInfo.isSelected) {
            //std::cout << "Entity " << entityID << " is selected!\n";
            entitySelectionInfo.selectedEntity = entityID;  // Store the selected entity's ID
        }
        else {
            entitySelectionInfo.selectedEntity = static_cast<EntityID>(-1);  // no entity is being selected
        }

        
    }


    void Collision_System::Update_Selected_Entity_Info(EntityID entityID, float entityX, float entityY, float entityWidth, float entityHeight)
    {

        Vec2D mousePos = Get_World_MousePos();
        g_selected_Entity_Info.mousePos = mousePos;

        bool isSelected = isInterseptBox(entityX, entityY, entityWidth, entityHeight, mousePos.x, mousePos.y);
        g_selected_Entity_Info.isSelected = isSelected;

        if (g_selected_Entity_Info.isSelected) {
            //std::cout << "Entity " << entityID << " is selected!\n";
            g_selected_Entity_Info.selectedEntity = entityID;  // Store the selected entity's ID
        }
        else {
            g_selected_Entity_Info.selectedEntity = static_cast<EntityID>(-1);  // no entity is being selected
        }


    }



    SelectedEntityInfo& Collision_System::get_selected_entity_info() {
        return g_selected_Entity_Info;
    }


    void Collision_System::MousePos_test()
    {
        /*double mouse_x;
        double mouse_y;*/

        if (!window)
        {
            return;
        }


        double screen_width = SM.get_scr_width();
        double screen_height = SM.get_scr_height(); //1020 /2
        //std::cout << "scene width: " << SM.get_scr_width() << " scene height: " << SM.get_scr_height() << "\n";
        // 1980 x 1020

        auto& camera = GFXM.get_camera();
        double world_x =  camera.pos_x;
        double world_y =  camera.pos_y;
        std::cout << "camera x: " << world_x << " " << "camera y: " << world_y << "\n";
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &world_y);
        mouse_x -= (screen_width / 2);
        //mouse_y -= (screen_height / 2);
        mouse_x = mouse_x;
       
    }
#endif

  


} // namespace lof


