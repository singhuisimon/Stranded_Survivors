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

// Include other necessary headers
#include "../Utility/Vector2D.h" //to get the library from vector2D.h
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"
#include "System.h"

//include standard header
#include <iostream>

namespace lof
{
	/**
	* @class AABB
	* @brief It represent the axis-aligned bounding boxes which determine whether the 2 game odject are overlapping each other.
	*/
	struct AABB {

		Vec2D min; // minimum point of the bounding box
		Vec2D max; // maximum point of the bounding box

		
		/**
		* @brief Constructor to initialize minimun and maximun points of AABB
		* @param min Minimum corner of the bounding box
		* @param max Maximum corner of the bounding box
		*/
		AABB(const Vec2D& min, const Vec2D& max);

		
		/**
		* @brief create AABB from transform component and collision component 
		* @param transform Tranform data including position, rotation, scale
		* @param collision Collision component data including width and height 
		* @return AABB instance 
		*/
		static AABB from_Tranform(const Transform2D& transform, const Collision_Component& collision);
	};
	

	/**
	* @class Collision System
	* @brief Handles collision detection between entities
	*/
	class Collision_System : public System
	{
	private:

		class ECS_Manager& ecs_manager;
	public:
		/**
		* @brief Collision System constructor which initialize the collision system with ECS manager
		* @param ecs_maager Reference to ECS manager
		*/
		Collision_System(ECS_Manager& ecs_manager);

		
		struct CollisionPair {
		EntityID entity1;
		EntityID entity2;
		Vec2D overlap;
		};

		


		/**
		* @brief Check for intersection between rectangles
		* @param aabb1 First AABB
		* @param vel1 First velocity
		* @param aabb2 Second AABB
		* @param vel2 Second AABB
		* @param firstTimeOfCollsion Output param to hold the time of collision
		* @return True if rectangle intersept, false otherwise
		*/
		bool Collision_Intersection_RectRect(const AABB& aabb1,
			const Vec2D& vel1,
			const AABB& aabb2,
			const Vec2D& vel2,
			float& firstTimeOfCollision);

		/**
		* @brief Check check if a point is within a box
		* @param pos_box_x Position of x for the rectangle
		* @param pos_box_y Position of y for the rectangle
		* @param width_box Width of the rectangle
		* @param height_box Height of the rectangle
		* @param mouseX Position x of the mouse
		* @param mouseY Position y of the mouse
		* @return True if mouse intersept the box, false otherwise
		*/
		bool is_Intersept_Box(float pos_box_x, float pos_box_y, float width_box, float height_box, int mouseX, int mouseY);

	

		/**
		* @brief Compute the overlap between AABBs. It determines the amount of overlap for both x-axis and y-axis.
		* @param aabb1 First aabb object
		* @param aabb2 Second aabb object
		* @return The Vec2D value can be use to resolve the collisions
		*/
		Vec2D Compute_Overlap(const AABB& aabb1, const AABB& aabb2);

		/**
		* @brief Check if the collsion is collide for every entities
		* @param collisions A references to a vector of Collision pair object for their overlap information
		* @param aabb2 Second aabb object
		* @param delta_time The time for the last update

		*/
		void Collision_Check_Collide(std::vector<CollisionPair>& collisions, float delta_time);


		/**
		* @brief Resolve the collison if they have colllied and update the position and velocities of involved entities
		* to resolve the collision and ensure they no longer overlap
		* @param collisions A references to a vector of Collision pair object for their overlap information

		*/
		void Resolve_Collsion_Event(const std::vector<CollisionPair>& collisions);

		/**
		* @brief Resolve the collision between an dynamic object and static object for rectangle based on AABB
		* @param aabb1 First aabb object
		* @param aabb2 Second aabb object
		* @param transform1 The transfrom (position) of dynamic object
		* @param transform2 The transform (position) of static object
		* @param Overlap The overlap between AABBs along the x and y axis
		* @return The Vec2D value can be use to resolve the collisions
		*/
#if 1
		void Resolve_Collision_Static_Dynamic(const AABB& aabb1, const AABB& aabb2, Transform2D& transform1, const Vec2D& overlap);
#endif
#if 0
		void Resolve_Collision_Static_Dynamic(const AABB& aabb1, const AABB& aabb2, Vec2D& transform1, const Vec2D& overlap);
#endif
#if 0
		void Resolve_Collision_Static_Dynamic(AABB& aabb1, const AABB& aabb2, Transform2D& transform1, Collision_Component& collision1, Velocity_Component& velocity, const Vec2D& Overlap);
#endif
		/**
		* @brief Update the collision system
		* @param delta_time Delta time since the last update. 
		*/
		void update(float delta_time) override;
		//might need: handle collision

		/**
		* @brief Returns the type of the collision system
		* @return string representing the type
		*/
		std::string get_type() const override;
	};
}


