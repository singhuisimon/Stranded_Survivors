/**
 * @file Collision_Syetem.cpp
 * @brief Implements the collsion system.
 * @author Saw Hui Shan (100%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.

 */
#if 0
 // Include standard headers
#include <iostream>

// Include headers
#include "Collision_System.h"

// Include other necessary headers
#include "../Component/Component.h"
#include "../System/Render_System.h"

// Initialize Value
float OVERLAP_X = 0.0f;
float OVERLAP_Y = 0.0f;


namespace lof
{
	/**
	* @brief Constructor to initialize minimun and maximun points of AABB
	* @param min Minimum corner of the bounding box
	* @param max Maximum corner of the bounding box
	*/
	AABB::AABB(const Vec2D& min, const Vec2D& max)
		: min(min), max(max) {}

	/**
	* @brief create AABB from transform component and collision component
	* @param transform Tranform data including position, rotation, scale
	* @param collision Collision component data including width and height
	* @return AABB instance
	*/
	AABB AABB::from_Tranform(const Transform2D& transform, const Collision_Component& collision)
	{
		Vec2D min;
		Vec2D max;

		min.x = transform.position.x - (collision.width / 2.0f);
		min.y = transform.position.y - (collision.height / 2.0f);

		max.x = transform.position.x + (collision.width / 2.0f);
		max.y = transform.position.y + (collision.height / 2.0f);

		return AABB(min, max); //return the created AABB
	}

	/**
	* @brief Check for intersection between rectangles
	* @param aabb1 First AABB
	* @param vel1 First velocity
	* @param aabb2 Second AABB
	* @param vel2 Second AABB
	* @param firstTimeOfCollsion Output param to hold the time of collision
	* @return True if rectangle intersept, false otherwise
	*/
	bool Collision_System::Collision_Intersection_RectRect(const AABB& aabb1,
		const Vec2D& vel1,
		const AABB& aabb2,
		const Vec2D& vel2,
		float& firstTimeOfCollision)
	{

		//check for A.max < B.min or A.min > B.min for x axis & y axis
		if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x ||
			aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y)
		{
			return false; //no intersection

		}


		float Vb_x = vel2.x - vel1.x;; //initialize Vb for x axis 
		float Vb_y = vel2.y - vel1.y;; //initialize Vb for y axis

		/*if (Vb_x == 0 && (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x)) return false;
		if (Vb_y == 0 && (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y)) return false;*/

		float dFirst_x = aabb1.max.x - aabb2.min.x;  //initialize for dFirst for x-axis
		float dFirst_y = aabb1.max.y - aabb2.min.y;; // initialize for dFrist for y axis
		float dLast_x = aabb1.min.x - aabb2.max.x;; //initialize for dLast for x axis
		float dLast_y = aabb1.min.y - aabb2.max.y;; //initialize for dLast for y axis


		float tFirst = firstTimeOfCollision = 0.0f; //Initialise t first
		float tLast = 1.0f; //initialize tLast and assume 1 as the time step (g_dt)

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



	/**
	* @brief Collision System constructor which initialize the collision system with ECS manager
	* @param ecs_maager Reference to ECS manager
	*/
	Collision_System::Collision_System(ECS_Manager& ecs_manager)
		: ecs_manager(ecs_manager) {
		set_time(0);
	}
	Collision_System::Collision_System(ECS_Manager& ecs_manager): ecs_manager(ecs_manager) {}

	//bool Collision_System::is_dynamic_object(EntityID entity_ID) {
	//	// Check if the object has velocity or other properties indicating it's dynamic
	//	auto& velocity = ecs_manager.get_component<Velocity_Component>(entity_ID);
	//	return velocity.velocity.x != 0 || velocity.velocity.y != 0;
	//}

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
	bool Collision_System::is_Intersept_Box(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
	{
		return mouseX >= box_x && mouseY <= (box_x + width) &&
			mouseY >= box_y && mouseY <= (box_y + height);
	}

	/**
	* @brief Compute the overlap between AABBs. It determines the amount of overlap for both x-axis and y-axis.
	* @param aabb1 First aabb object
	* @param aabb2 Second aabb object
	* @return The Vec2D value can be use to resolve the collisions
	*/
	Vec2D Collision_System::Compute_Overlap(const AABB& aabb1, const AABB& aabb2)
	{
		//float Overlap_x = 0.0f, Overlap_y = 0.0f;

		//calculta the overlap for x axis, check if any overlap on the x-axis
		if (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x)
		{
			float overlap_x1 = aabb1.max.x - aabb2.min.x;
			float overlap_x2 = aabb2.max.x - aabb1.min.x;
			if (overlap_x1 < overlap_x2) {
				OVERLAP_X = overlap_x1;
			}
			else {
				OVERLAP_X = overlap_x2;
			}
		}

		//calculta the overlap for y axis
		if (aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y)
		{
			float overlap_y1 = aabb1.max.y - aabb2.min.y;
			float overlap_y2 = aabb2.max.y - aabb1.min.y;

			if (overlap_y1 < overlap_y2) {
				OVERLAP_Y = overlap_y1;
			}
			else {
				OVERLAP_Y = overlap_y2;
			}
		}

		return Vec2D(OVERLAP_X, OVERLAP_Y);
	}




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
	void Collision_System::Resolve_Collision_Static_Dynamic(const AABB& aabb1, const AABB& aabb2, Transform2D& transform1, const Vec2D& Overlap)
	{

		bool resolve_x = (Overlap.x < Overlap.y);

		if (resolve_x) {
			// Resolve in the x direction
			if (aabb1.min.x < aabb2.min.x) { // dynamic object move from left to right at the left side of static object 

				transform1.position.x -= Overlap.x;

			}
			else if (aabb1.max.x > aabb2.min.x) { // dynamic object move from right to left at the right side of static object 

				transform1.position.x += Overlap.x;
			}

		}
		else {

			if (aabb1.min.y < aabb2.min.y) { // dynamic object move from bottom to top at the bottom side of static object 

				transform1.position.y -= Overlap.y;

			}
			else if (aabb1.max.y > aabb2.min.y) { // dynamic object move from top to bottom at the top side of static object 
				// aabb1 is below aabb2
				transform1.position.y += Overlap.y;
			}
		}
	}
#endif
#if 0
	void Collision_System::Resolve_Collision_Static_Dynamic(const AABB& aabb1, const AABB& aabb2, Vec2D& transform1, const Vec2D& Overlap)
	{

		bool resolve_x = (Overlap.x < Overlap.y);

		if (resolve_x) {
			// Resolve in the x direction
			if (aabb1.min.x < aabb2.min.x) { // dynamic object move from left to right at the left side of static object 

				transform1.x -= Overlap.x;

			}
			else if (aabb1.max.x > aabb2.min.x) { // dynamic object move from right to left at the right side of static object 

				transform1.x += Overlap.x;


			}
		}
		else {

			if (aabb1.min.y < aabb2.min.y) { // dynamic object move from bottom to top at the bottom side of static object 

				transform1.y -= Overlap.y;

			}
			else if (aabb1.max.y > aabb2.min.y) { // dynamic object move from top to bottom at the top side of static object 
				// aabb1 is below aabb2
				transform1.y += Overlap.y;
			}
		}
	}
#endif

	/**
	* @brief Check if the collsion is collide for every entities 
	* @param collisions A references to a vector of Collision pair object for their overlap information
	* @param aabb2 Second aabb object
	* @param delta_time The time for the last update

	*/
	void Collision_System::Collision_Check_Collide(std::vector<CollisionPair>& collisions, float delta_time)
	{
		AABB aabb1{ {0,0}, {0,0} };
		AABB aabb2{ {0,0}, {0,0} };

		// Iterate all the entities in ecs_manager
		for (const auto& entity_ptr : ecs_manager.get_entities())
		{
			EntityID entity_ID = entity_ptr->get_id();
			auto& physic1 = ecs_manager.get_component<Physics_Component>(entity_ID);

			// Check if entity has a collision component and is dynamic
			if (entity_ptr->has_component(ecs_manager.get_component_id<Collision_Component>()) && !physic1.is_static)
			{
				auto& transform1 = ecs_manager.get_component<Transform2D>(entity_ID);
				auto& collision1 = ecs_manager.get_component<Collision_Component>(entity_ID);
				auto& velocity1 = ecs_manager.get_component<Velocity_Component>(entity_ID);

				// Create AABB for object 1
				aabb1 = AABB::from_Tranform(transform1, collision1);

				// Check for collisions with other entities
				for (const auto& other_entity_ptr : ecs_manager.get_entities())
				{
					// Skip if it's the same entity
					if (entity_ptr == other_entity_ptr)
						continue;

					// Check if other entity has a collision component
					if (other_entity_ptr->has_component(ecs_manager.get_component_id<Collision_Component>()))
					{
						EntityID Other_Entity_ID = other_entity_ptr->get_id();
						auto& transform2 = ecs_manager.get_component<Transform2D>(Other_Entity_ID);
						auto& collision2 = ecs_manager.get_component<Collision_Component>(Other_Entity_ID);
						auto& velocity2 = ecs_manager.get_component<Velocity_Component>(Other_Entity_ID);

						// Create AABB for the other entity
						aabb2 = AABB::from_Tranform(transform2, collision2);

						// Check for intersection between two entities
						float collision_time = delta_time;
						if (Collision_Intersection_RectRect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time))
						{
							// Store collision pair and overlap information
							collisions.push_back({ entity_ID, Other_Entity_ID, Compute_Overlap(aabb1, aabb2) });
						}
					}
				}
			}
		}

	}

	/**
	* @brief Resolve the collison if they have colllied and update the position and velocities of involved entities
	* to resolve the collision and ensure they no longer overlap
	* @param collisions A references to a vector of Collision pair object for their overlap information

	*/
	void Collision_System::Resolve_Collsion_Event(const std::vector<CollisionPair>& collisions)
	{
		for (const auto& collision : collisions)
		{

			auto& transform1 = ecs_manager.get_component<Transform2D>(collision.entity1);
			auto& velocity1 = ecs_manager.get_component<Velocity_Component>(collision.entity1);
			auto& physic1 = ecs_manager.get_component<Physics_Component>(collision.entity1);

			auto& transform2 = ecs_manager.get_component<Transform2D>(collision.entity2);
			auto& velocity2 = ecs_manager.get_component<Velocity_Component>(collision.entity2);
			auto& physic2 = ecs_manager.get_component<Physics_Component>(collision.entity2);

			AABB first_form = AABB::from_Tranform(transform1, collision.entity1);

			AABB second_form = AABB::from_Tranform(transform2, collision.entity2);

			if (collision.overlap.y > 0) {
				velocity1.velocity.y = 0.0f;  // Stop falling when hitting platform
				physic1.is_grounded = true;   // Entity is now grounded
			}

			if (collision.overlap.x > 0 && collision.overlap.y > 0)
			{
				// Collision detected, stop both objects
				velocity1.velocity.x = 0.0f; 
				velocity1.velocity.y = 0.0f; 
				physic1.is_grounded = true;
			}
			// Resolve the collision
			Resolve_Collision_Static_Dynamic(first_form, second_form , transform1, collision.overlap);

			// Additional handling after resolution (e.g., setting grounded status)
			if (physic1.is_grounded)
			{
				physic1.is_jumping = false;
			}
			
		}
	}
	/**
	* @brief Update the collision system
	* @param delta_time Delta time since the last update.
	*/
#if 1
	void Collision_System::update(float delta_time)
	{
		std::vector<CollisionPair> collisions;
		Collision_System::Collision_Check_Collide(collisions, delta_time); // Check for collisions and fill the collision list
		Collision_System::Resolve_Collsion_Event(collisions);
	}
#endif

#if 0
	void Collision_System::update(float delta_time)
	{
		for (const auto& entity_ptr : ecs_manager.get_entities())
		{
			EntityID Entity_ID = entity_ptr->get_id();

			// Get components
			Physics_Component& all_entities_physic = ecs_manager.get_component<Physics_Component>(Entity_ID);
			Velocity_Component& all_entities_velocity = ecs_manager.get_component<Velocity_Component>(Entity_ID);
			Collision_Component& all_entities_collision = ecs_manager.get_component<Collision_Component>(Entity_ID);
			Transform2D& all_entities_transform = ecs_manager.get_component<Transform2D>(Entity_ID);

			// Player-specific variables
			Vec2 Player_velocity = { 0.0f, 0.0f };
			Vec2 Player_transform = { 0.0f, 0.0f };
			bool Player_physic_is_grounded = false;
			bool Player_physic_is_jumped = false;
			AABB aabb1(Vec2D{ 0, 0 }, Vec2D{ 0, 0 });

			// Handle player-specific logic (Player ID = 4)
			if (Entity_ID == 4)
			{
				Player_velocity = all_entities_velocity.velocity;
				Player_transform = all_entities_transform.position;
				Player_physic_is_grounded = all_entities_physic.is_grounded;
				Player_physic_is_jumped = all_entities_physic.is_jumping;

				// Create player's AABB
				aabb1 = AABB::from_Tranform(all_entities_transform, all_entities_collision);
			}

			// Non-player entities
			Vec2 other_velocity = { 0.0f, 0.0f };
			Vec2 other_transform = { 0.0f, 0.0f };
			AABB aabb2(Vec2D{ 0, 0 }, Vec2D{ 0, 0 });

			if (Entity_ID != 4)
			{
				other_transform = all_entities_transform.position;
				other_velocity = all_entities_velocity.velocity;

				// Create other entity's AABB
				aabb2 = AABB::from_Tranform(all_entities_transform, all_entities_collision);
			}

			// Check for collisions and overlap resolution
			float collision_time = delta_time;
			if (Entity_ID != 4 && Collision_Intersection_RectRect(aabb1, Player_velocity, aabb2, other_velocity, collision_time))
			{
				Vec2D Overlap = Compute_Overlap(aabb1, aabb2);

				// Handle vertical overlap (landing or falling)
				if (Overlap.y > 0 && Player_velocity.y > 0)  // Player is falling and hits the platform
				{
					// Stop vertical velocity
					Player_velocity.y = 0.0f;
					// Mark the player as grounded
					Player_physic_is_grounded = true;
					// Adjust player's position to not overlap
					Player_transform.y -= Overlap.y;
				}

				// Handle horizontal overlap (bumping into objects)
				if (Overlap.x > 0)
				{
					Player_velocity.x = 0.0f;  // Stop horizontal movement
				}

				// Update jump status
				if (Player_physic_is_grounded)
				{
					Player_physic_is_jumped = false;
				}
			}

			// Apply gravity if the player is not grounded
			//if (Entity_ID == 4 && !Player_physic_is_grounded)
			//{
			//	// Apply gravity to the player's velocity
			//	Player_velocity.y += GRAVITY * delta_time;
			//}

			// Update the player's position
			all_entities_transform.position.x += Player_velocity.x * delta_time;
			all_entities_transform.position.y += Player_velocity.y * delta_time;

			// Update the player's grounded state in the Physics component
			if (Entity_ID == 4)
			{
				all_entities_physic.is_grounded = Player_physic_is_grounded;
				all_entities_physic.is_jumping = Player_physic_is_jumped;
			}
		}
	}



#endif
	/**
	* @brief Returns the type of the collision system
	* @return string representing the type
	*/
	std::string Collision_System::get_type() const {
		return "Collision_System";
	}

}
#endif
/**
 * @file Collision_Syetem.cpp
 * @brief Implements the collsion system.
 * @author Saw Hui Shan
 * @date September 21, 2024
 */

 // Include standard headers
#include <iostream>

// Include headers
#include "Collision_System.h"

// Include other necessary headers
#include "../Component/Component.h"
#include "../System/Render_System.h"

// Initialize Value
float OVERLAP_X = 0.0f;
float OVERLAP_Y = 0.0f;


namespace lof
{
	/**
	* @brief Constructor to initialize minimun and maximun points of AABB
	* @param min Minimum corner of the bounding box
	* @param max Maximum corner of the bounding box
	*/
	AABB::AABB(const Vec2D& min, const Vec2D& max)
		: min(min), max(max) {}

	/**
	* @brief create AABB from transform component and collision component
	* @param transform Tranform data including position, rotation, scale
	* @param collision Collision component data including width and height
	* @return AABB instance
	*/
	AABB AABB::from_Tranform(const Transform2D& transform, const Collision_Component& collision)
	{
		Vec2D min;
		Vec2D max;

		min.x = transform.position.x - (collision.width / 2.0f);
		min.y = transform.position.y - (collision.height / 2.0f);

		max.x = transform.position.x + (collision.width / 2.0f);
		max.y = transform.position.y + (collision.height / 2.0f);

		return AABB(min, max); //return the created AABB
	}

	/**
	* @brief Check for intersection between rectangles
	* @param aabb1 First AABB
	* @param vel1 First velocity
	* @param aabb2 Second AABB
	* @param vel2 Second AABB
	* @param firstTimeOfCollsion Output param to hold the time of collision
	* @return True if rectangle intersept, false otherwise
	*/
	bool Collision_System::Collision_Intersection_RectRect(const AABB& aabb1,
		const Vec2D& vel1,
		const AABB& aabb2,
		const Vec2D& vel2,
		float& firstTimeOfCollision)
	{

		//check for A.max < B.min or A.min > B.min for x axis & y axis
		if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x ||
			aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y)
		{
			return false; //no intersection

		}


		float Vb_x = vel2.x - vel1.x;; //initialize Vb for x axis 
		float Vb_y = vel2.y - vel1.y;; //initialize Vb for y axis

		/*if (Vb_x == 0 && (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x)) return false;
		if (Vb_y == 0 && (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y)) return false;*/

		float dFirst_x = aabb1.max.x - aabb2.min.x;  //initialize for dFirst for x-axis
		float dFirst_y = aabb1.max.y - aabb2.min.y;; // initialize for dFrist for y axis
		float dLast_x = aabb1.min.x - aabb2.max.x;; //initialize for dLast for x axis
		float dLast_y = aabb1.min.y - aabb2.max.y;; //initialize for dLast for y axis


		float tFirst = firstTimeOfCollision = 0.0f; //Initialise t first
		float tLast = 1.0f; //initialize tLast and assume 1 as the time step (g_dt)

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



	/**
	* @brief Collision System constructor which initialize the collision system with ECS manager
	* @param ecs_maager Reference to ECS manager
	*/
	Collision_System::Collision_System(ECS_Manager& ecs_manager)
		: ecs_manager(ecs_manager) {
		set_time(0);
	}

	//bool Collision_System::is_dynamic_object(EntityID entity_ID) {
	//	// Check if the object has velocity or other properties indicating it's dynamic
	//	auto& velocity = ecs_manager.get_component<Velocity_Component>(entity_ID);
	//	return velocity.velocity.x != 0 || velocity.velocity.y != 0;
	//}

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
	bool Collision_System::is_Intersept_Box(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
	{
		return mouseX >= box_x && mouseY <= (box_x + width) &&
			mouseY >= box_y && mouseY <= (box_y + height);
	}

	/**
	* @brief Compute the overlap between AABBs. It determines the amount of overlap for both x-axis and y-axis.
	* @param aabb1 First aabb object
	* @param aabb2 Second aabb object
	* @return The Vec2D value can be use to resolve the collisions
	*/
	Vec2D Collision_System::Compute_Overlap(const AABB& aabb1, const AABB& aabb2)
	{
		//float Overlap_x = 0.0f, Overlap_y = 0.0f;

		//calculta the overlap for x axis, check if any overlap on the x-axis
		if (aabb1.min.x < aabb2.max.x && aabb1.max.x > aabb2.min.x)
		{
			float overlap_x1 = aabb1.max.x - aabb2.min.x;
			float overlap_x2 = aabb2.max.x - aabb1.min.x;
			if (overlap_x1 < overlap_x2) {
				OVERLAP_X = overlap_x1;
			}
			else {
				OVERLAP_X = overlap_x2;
			}
		}

		//calculta the overlap for y axis
		if (aabb1.min.y < aabb2.max.y && aabb1.max.y > aabb2.min.y)
		{
			float overlap_y1 = aabb1.max.y - aabb2.min.y;
			float overlap_y2 = aabb2.max.y - aabb1.min.y;

			if (overlap_y1 < overlap_y2) {
				OVERLAP_Y = overlap_y1;
			}
			else {
				OVERLAP_Y = overlap_y2;
			}
		}

		return Vec2D(OVERLAP_X, OVERLAP_Y);
	}



	/**
	* @brief Resolve the collision between an dynamic object and static object for rectangle based on AABB
	* @param aabb1 First aabb object
	* @param aabb2 Second aabb object
	* @param transform1 The transfrom (position) of dynamic object
	* @param transform2 The transform (position) of static object
	* @param Overlap The overlap between AABBs along the x and y axis
	* @return The Vec2D value can be use to resolve the collisions
	*/

	void Collision_System::Resolve_Collision_Static_Dynamic(const AABB& aabb1, const AABB& aabb2, Transform2D& transform1, const Vec2D& Overlap)
	{
#if 0
		bool resolve_x = (Overlap.x < Overlap.y);

		if (resolve_x) {
			// Resolve in the x direction
			if (aabb1.min.x < aabb2.min.x) { // dynamic object move from left to right at the left side of static object 

				transform1.position.x -= Overlap.x;

			}
			else if (aabb1.max.x > aabb2.min.x) { // dynamic object move from right to left at the right side of static object 

				transform1.position.x += Overlap.x;


			}
		}
		else {

			if (aabb1.min.y < aabb2.min.y) { // dynamic object move from bottom to top at the bottom side of static object 

				transform1.position.y -= Overlap.y;

			}
			else if (aabb1.max.y > aabb2.min.y) { // dynamic object move from top to bottom at the top side of static object 
				// aabb1 is below aabb2
				transform1.position.y += Overlap.y;
			}
		}
#endif
		

	}

	

	/**
	* @brief Update the collision system
	* @param delta_time Delta time since the last update.
	*/
	void Collision_System::update(float delta_time)
	{
		//Iterate all the entities in ecs_manager
		for (const auto& entity_ptr : ecs_manager.get_entities())
		{
			//get the ID of the entity
			EntityID entity_ID = entity_ptr->get_id();
			//LM.write_log("Entity id %u",entity_ID);

			//std::cout << entity_ID << "\n";

			auto& physic1 = ecs_manager.get_component<Physics_Component>(entity_ID);

			//check if entity has a collision component
			if (entity_ptr->has_component(ecs_manager.get_component_id< Collision_Component>()) && !physic1.is_static) {
				// LM.write_log("Entity id %u", entity_ID);

				//get transform component
				auto& transform1 = ecs_manager.get_component<Transform2D>(entity_ID);
				//get collision component
				auto& collision1 = ecs_manager.get_component<Collision_Component>(entity_ID);
				//get velocity component
				auto& velocity1 = ecs_manager.get_component<Velocity_Component>(entity_ID);

				//auto& physic1 = ecs_manager.get_component<Physics_Component>(entity_ID);


				//create AABB based on transforrm and collision component for object 1
				AABB aabb1 = AABB::from_Tranform(transform1, collision1);

				//check for collision for all other entities
				for (const auto& other_entity_ptr : ecs_manager.get_entities())
				{
					//if they are the same entity skip
					if (entity_ptr == other_entity_ptr)
					{
						continue;
					}

					//check if other entity has a collision component
					if (other_entity_ptr->has_component(ecs_manager.get_component_id<Collision_Component>()))
					{
						EntityID Other_Entity_ID = other_entity_ptr->get_id();

						//get other transform component 
						auto& transform2 = ecs_manager.get_component<Transform2D>(Other_Entity_ID);

						//get collision component
						auto& collision2 = ecs_manager.get_component<Collision_Component>(Other_Entity_ID);

						//get velocity component
						auto& velocity2 = ecs_manager.get_component<Velocity_Component>(Other_Entity_ID);
						auto& physic2 = ecs_manager.get_component<Physics_Component>(Other_Entity_ID);



						//create AABB for other entity
						AABB aabb2 = AABB::from_Tranform(transform2, collision2);
						// LM.write_log("AABB 2: Min(%f, %f) Max(%f, %f) from entity %u", aabb2.min.x, aabb2.min.y, aabb2.max.x, aabb2.max.y, Other_entity_ID);

						float collision_time = delta_time;// 0.0f;

						//check intersecption between two entities, and consider their vel
						if (Collision_Intersection_RectRect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time))
						{
							
							//if (velocity1.velocity.x == 0.0f && velocity1.velocity.y == 0.0f &&
							//	velocity2.velocity.x == 0.0f && velocity2.velocity.y == 0.0f) {
							//	// If both objects are stationary, skip collision check
							//	continue;
							//}
							Vec2D Overlap = Compute_Overlap(aabb1, aabb2);

							if (Other_Entity_ID == 1 || Other_Entity_ID == 2)
							{
								physic1.is_grounded = true;
								if (!physic1.is_jumping && physic1.is_grounded == false)
								{
									physic1.gravity.y = 0.0f; // Stop applying gravity while grounded
									velocity1.velocity.y = 0.0f; // Stop vertical movemen

								}
								

								Resolve_Collision_Static_Dynamic(aabb1, aabb2, transform1, Overlap);
							
							}
							else {
								physic1.is_grounded = false;
								physic1.gravity.y = 0.0f;
							}
							
							
							//std::cout << "yes !!!is collide\n";
							

							/*if (physic1.is_static && physic2.is_static)
							{
								physic1.is_grounded = true;
								continue;
							}*/

							if (IM.is_key_held(GLFW_KEY_M)) {
								LM.write_log("Collision_Syetem::update():Yes, Collision is detected.");
							}

							
							

							//if (Overlap.y > 0 && transform1.position.y < transform2.position.y) { // Player on top of a platform
							//	velocity1.velocity.y = 0.0f;  // Stop falling
							//	physic1.is_grounded = true;   // Entity is grounded
							//}



						//	if (Other_Entity_ID == 3 && !physic1.is_static)
						//	{

						//		Resolve_Collision_Static_Dynamic(aabb1, aabb2, transform1, Overlap);
						//	}

						//	if (Overlap.y > 0) {
						//		velocity1.velocity.y = 0.0f;  // Stop falling when hitting platform
						//		//physic1.is_grounded = true;   // Entity is now grounded
						//	}

						//	// Determine if there is an overlap
						//	if (Overlap.x > 0 && Overlap.y > 0)
						//	{
						//		// Collision detected, stop both objects
						//		velocity1.velocity.x = 0.0f; // Stop the first object's horizontal movement
						//		velocity1.velocity.y = 0.0f; // Stop the first object's vertical movement
						//	
						//		
						//		Resolve_Collision_Static_Dynamic(aabb1, aabb2, transform1, Overlap);
						//	
						//	}

						//	if (Overlap.y > 0 && transform1.position.y < transform2.position.y) {
						//		velocity1.velocity.y = 0.0f;  // Stop falling
						//		physic1.is_grounded = true;   // Entity is grounded
						//	}

						//}

						//else if (IM.is_key_held(GLFW_KEY_M))
						//{
						//	LM.write_log("Collision_Syetem::update():No, Collision is not detected.");
						//}
						// }
						}
						/*else {
							physic1.is_grounded = false;
							
						}*/
						
						
					}
					std::cout << "gravity of player 1 " << physic1.gravity.x << "   " << physic1.gravity.y << "\n";
					std::cout << "physic is grounded for player 1 in collisison " << physic1.is_grounded << "\n";

				}
			}
		}
	}

	/**
	* @brief Returns the type of the collision system
	* @return string representing the type
	*/
	std::string Collision_System::get_type() const {
		return "Collision_System";
	}

}

