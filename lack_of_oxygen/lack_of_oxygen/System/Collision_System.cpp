/**
 * @file Collision_Syetem.cpp
 * @brief Implements the collsion system.
 * @author Saw Hui Shan
 * @date September 21, 2024
 */

//include the header file
#include "Collision_System.h"
#include "../Component/Component.h"
#include <iostream>
#include "../System/Render_System.h"
#include "../Utility/Constant.h"


namespace lof
{
	//constructor for AABB
	AABB::AABB(const Vec2D& min, const Vec2D& max)
		: min(min), max(max) {}

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

	Collision_System::Collision_System(ECS_Manager& ecs_manager)
		: ecs_manager(ecs_manager) {
		set_time(DEFAULT_START_TIME);
	}

	bool Collision_System::is_Intersept_Box(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
	{
		return mouseX >= box_x && mouseY <= (box_x + width) &&
			mouseY >= box_y && mouseY <= (box_y + height);
	}

	
	void Collision_System::update(float dt)
	{
		//Iterate all the entities in ecs_manager
		for (const auto& entity_ptr : ecs_manager.get_entities())
		{
			//get the ID of the entity
			EntityID entity_ID = entity_ptr->get_id();

			//
			//LM.write_log("Entity id %u",entity_ID);
			
			//ecs_manager.add_component<Collision_Component>(entity_ID, Collision_Component{ 500.0f,500.0f });
			//check if entity has a collision component
			if (entity_ptr->has_component(ecs_manager.get_component_id< Collision_Component>())) {
				// LM.write_log("Entity id %u", entity_ID);

				//get transform component
				auto& transform1 = ecs_manager.get_component<Transform2D>(entity_ID);
				//LM.write_log("transform 1 componenet for first entity id x:%f y:%f from entity: %u", transform1.position.x, transform1.position.y, entity_ID);
				//get collision component
				auto& collision1 = ecs_manager.get_component<Collision_Component>(entity_ID);
				

				//get velocity component
				auto& velocity1 = ecs_manager.get_component<Velocity_Component>(entity_ID);
				//LM.write_log("velocity componenet for first entity id x:%f y:%f from entity: %u", velocity1.velocity.x, velocity1.velocity.y, entity_ID);
				//std::cout << "testing:" << entity_ID;
				//create AABB based on transforrm and collision component
				AABB aabb1 = AABB::from_Tranform(transform1, collision1);
				//LM.write_log("AABB1 x-axis will be min: %f max: %f  from entity: %u ", aabb1.min.x, aabb1.min.x, entity_ID);
				//LM.write_log("AABB1 y-axis will be min: %f max: %f  from entity: %u", aabb1.max.y, aabb1.max.y, entity_ID);
				// LM.write_log("AABB 1: Min(%f, %f) Max(%f, %f) from entity %u", aabb1.min.x, aabb1.min.y, aabb1.max.x, aabb1.max.y,entity_ID);
				//LM.write_log("AABB 1: Min(%f, %f) Max(%f, %f)", aabb1.min.x, aabb1.min.y, aabb1.max.x, aabb1.max.y);
				
				//check for collision for all other entities
				for (const auto& other_entity_ptr : ecs_manager.get_entities())
				{
					//if they are the same entity skip
					if (entity_ptr == other_entity_ptr)
					{
						//LM.write_log("yes they are same entity %u and &u!!!!!!");
						continue;
					}

					//check if other entity has a collision component
					if (other_entity_ptr->has_component(ecs_manager.get_component_id<Collision_Component>()))
					{
						//LM.write_log("yes other entity!!!!!!");
						EntityID Other_entity_ID = other_entity_ptr->get_id();
						// LM.write_log("Other Entity id %u", Other_entity_ID);
						//get other transform component 
						auto& other_transform1 = ecs_manager.get_component<Transform2D>(Other_entity_ID);
						//get collision component
						auto& other_collision1 = ecs_manager.get_component<Collision_Component>(Other_entity_ID);

						//get velocity component
						auto& other_velocity1 = ecs_manager.get_component<Velocity_Component>(Other_entity_ID);
						//LM.write_log("velocity componenet for other  x:%f y:%f from entity: %u", other_velocity1.velocity.x, other_velocity1.velocity.y, Other_entity_ID);

						//std::cout << "testing:" << Other_entity_ID;

						//create AABB for other entity
						AABB aabb2 = AABB::from_Tranform(other_transform1, other_collision1);
						// LM.write_log("AABB 2: Min(%f, %f) Max(%f, %f) from entity %u", aabb2.min.x, aabb2.min.y, aabb2.max.x, aabb2.max.y, Other_entity_ID);
						float collision_time = 0.0f;

						//check intersecption between two entities, and consider their vel
						if (Collision_Intersection_RectRect(aabb1, velocity1.velocity, aabb2, other_velocity1.velocity, collision_time))
						{
							//std::cout << "yes, is collide\n"; //print to test first

							if (velocity1.velocity.x == 0.0f && velocity1.velocity.y == 0.0f &&
								other_velocity1.velocity.x == 0.0f && other_velocity1.velocity.y == 0.0f) {
								// If both objects are stationary, skip collision check
								continue;
							}

							LM.write_log("yes! it collide");
							std::cout << "yes !!! collide\n";
						}
					
					}

				}

			}

		}

	}

	std::string Collision_System::get_type() const {
		return "Collision_System";
	}

	//******************************************Original Collision_System::update code*********************************//

	//void Collision_System::update( float dt)
	//{
	//	//Iterate all the entities in ecs_manager
	//	for (const auto& entity_ptr : ecs_manager.get_entities())
	//	{
	//		//get the ID of the entity
	//		EntityID entity_ID = entity_ptr->get_id();
	//		//check if entity has a collision component
	//		if (entity_ptr->has_component(ecs_manager.get_component_id< Collision_Component>())) {
	//			
	//			//get transform component
	//			auto& transform1 = ecs_manager.get_component<Transform2D>(entity_ID);
	//			//get collision component
	//			auto& collision1 = ecs_manager.get_component<Collision_Component>(entity_ID);

	//			//get velocity component
	//			auto& velocity1 = ecs_manager.get_component<Velocity_Component>(entity_ID);

	//			//std::cout << "testing:" << entity_ID;
	//			//create AABB based on transforrm and collision component
	//			AABB aabb1 = AABB::from_Tranform(transform1, collision1);

	//			//check for collision for all other entities
	//			for (const auto& other_entity_ptr : ecs_manager.get_entities())
	//			{
	//				//if they are the same entity skip
	//				if (entity_ptr == other_entity_ptr)
	//				{
	//					continue;
	//				}

	//				//check if other entity has a collision component
	//				if (other_entity_ptr->has_component(ecs_manager.get_component_id<Collision_Component>()))
	//				{
	//					EntityID Other_entity_ID = other_entity_ptr->get_id();
	//					//get other transform component 
	//					auto& other_transform1 = ecs_manager.get_component<Transform2D>(Other_entity_ID);
	//					//get collision component
	//					auto& other_collision1 = ecs_manager.get_component<Collision_Component>(Other_entity_ID);

	//					//get velocity component
	//					auto& other_velocity1 = ecs_manager.get_component<Velocity_Component>(Other_entity_ID);

	//					std::cout << "testing:" << Other_entity_ID;

	//					//create AABB for other entity
	//					AABB aabb2 = AABB::from_Tranform(other_transform1, other_collision1);
	//					float collision_time = 0.0f;

	//					//check intersecption between two entities, and consider their vel
	//					if (Collision_Intersection_RectRect(aabb1, velocity1.velocity, aabb2, other_velocity1.velocity, collision_time))
	//					{
	//						std::cout << "yes, is collide\n"; //print to test first
	//					}

	//				}

	//			}

	//		}

	//	}
	//	
	//}

	//******************************************Original Collision_System::update code*********************************//

}

