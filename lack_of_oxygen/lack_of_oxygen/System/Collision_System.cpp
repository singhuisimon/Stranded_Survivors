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
	void Collision_System::Resolve_Collision_Static_Dynamic(const AABB& aabb1, const AABB& aabb2, Transform2D& transform1, Transform2D& transform2, const Vec2D& Overlap)
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
				std::cout << "it is less aabb1 test tes teststette   \n";
				
			}
			else if (aabb1.max.y > aabb2.min.y) { // dynamic object move from top to bottom at the top side of static object 
				// aabb1 is below aabb2
				transform1.position.y += Overlap.y; 
			}
		}
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

			//check if entity has a collision component
			if (entity_ptr->has_component(ecs_manager.get_component_id< Collision_Component>())) {
				// LM.write_log("Entity id %u", entity_ID);

				//get transform component
				auto& transform1 = ecs_manager.get_component<Transform2D>(entity_ID);
				//get collision component
				auto& collision1 = ecs_manager.get_component<Collision_Component>(entity_ID);
				//get velocity component
				auto& velocity1 = ecs_manager.get_component<Velocity_Component>(entity_ID);
				

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
						
						//create AABB for other entity
						AABB aabb2 = AABB::from_Tranform(transform2, collision2);
						// LM.write_log("AABB 2: Min(%f, %f) Max(%f, %f) from entity %u", aabb2.min.x, aabb2.min.y, aabb2.max.x, aabb2.max.y, Other_entity_ID);

						float collision_time = 0.0f;

						//check intersecption between two entities, and consider their vel
						if (Collision_Intersection_RectRect(aabb1, velocity1.velocity, aabb2, velocity2.velocity, collision_time))
						{

							if (velocity1.velocity.x == 0.0f && velocity1.velocity.y == 0.0f &&
								velocity2.velocity.x == 0.0f && velocity2.velocity.y == 0.0f) {
								// If both objects are stationary, skip collision check
								continue;
							}

							LM.write_log("yes! It is collide");
							
							Vec2D Overlap = Compute_Overlap(aabb1, aabb2);

							// Determine if there is an overlap
							if (Overlap.x > 0 && Overlap.y > 0) 
							{
								// Collision detected, stop both objects
								velocity1.velocity.x = 0.0f; // Stop the first object's horizontal movement
								velocity1.velocity.y = 0.0f; // Stop the first object's vertical movement


								Resolve_Collision_Static_Dynamic(aabb1, aabb2, transform1, transform2, Overlap);

							}

						}

					}

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

