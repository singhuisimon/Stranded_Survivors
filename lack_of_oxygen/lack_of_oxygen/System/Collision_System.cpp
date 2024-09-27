#include "Collision_System.h"
#include "../Component/Component.h"

namespace lof
{

	//default constructor -  initialize the width height 
	//Collision_Component::Collision_Component(float width = 0.0f, float height = 0.0f)
	//	: width(width), height(height) {
	//	std::cout << "test for print\n";
	//}

	////constructor for Transform_Component
	//Transform_Component::Transform_Component(float x, float y, float width, float height, float rotation)
	//	: pos(x, y), m_width(width), m_height(height), m_rotation(rotation) {}

	//constructor for AABB
	AABB::AABB(const Vec2D& min, const Vec2D& max)
		: min(min), max(max) {}

	//AABB AABB::from_Tranform(const Transform_Component& transform)
	//{
	//	Vec2D min;
	//	Vec2D max;

	//	//get the 
	//	min.x = transform.pos.x - (transform.m_width / 2.0f);
	//	min.y = transform.pos.y - (transform.m_height / 2.0f);
	//	max.x = transform.pos.x - (transform.m_width / 2.0f);
	//	max.y = transform.pos.x - (transform.m_height / 2.0f);

	//	return AABB(min, max);
	//	//calclutate the min and max point based on position and size
	//}

	void printout()
	{
		std::cout << "output some to see if got anything on console\n";
	}

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

	bool Collision_System::is_Intersept_Box(float box_x, float box_y, float width, float height, int mouseX, int mouseY)
	{
		return mouseX >= box_x && mouseY <= (box_x + width) &&
			mouseY >= box_y && mouseY <= (box_y + height);
	}

	void Collision_System::Collision_Update( float dt)
	{
		//Iterate all the entities in ecs_manager
		for (const auto& entity_ptr : ecs_manager.get_entities())
		{
			//get the ID of the entity
			EntityID entity_ID = entity_ptr->get_id();
			//check if entity has a collision component
			if (entity_ptr->has_component(ecs_manager.get_component_id< Collision_Component>())) {
				
				//get transform component
				auto& transform1 = ecs_manager.get_component<Transform2D>(entity_ID);
				//get collision component
				auto& collision1 = ecs_manager.get_component<Collision_Component>(entity_ID);

				//get velocity component
				auto& velocity1 = ecs_manager.get_component<Velocity_Component>(entity_ID);

				std::cout << "testing:" << entity_ID;


				//create AABB based on transforrm and collision component
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
						EntityID Other_entity_ID = other_entity_ptr->get_id();
						//get other transform component 
						auto& other_transform1 = ecs_manager.get_component<Transform2D>(Other_entity_ID);
						//get collision component
						auto& other_collision1 = ecs_manager.get_component<Collision_Component>(Other_entity_ID);

						//get velocity component
						auto& other_velocity1 = ecs_manager.get_component<Velocity_Component>(Other_entity_ID);

						std::cout << "testing:" << Other_entity_ID;

						


						//create AABB for other entity
						AABB aabb2 = AABB::from_Tranform(other_transform1, other_collision1);
						float collision_time = 0.0f;

						//check intersecption between two entities, and consider their vel
						if (Collision_Intersection_RectRect(aabb1, velocity1.velocity, aabb2, other_velocity1.velocity, collision_time))
						{
							std::cout << "yes, is collide\n"; //print to test first
						}

					}

				}

			}

		}
		
	}


	//bool Check_Collision(const std::vector<Entity>& entity,
	//	const std::vector<Position_Component>& pos,
	//	const std::vector<Velocity_Component>& vel)
	//{
	//	for (size_t i = 0; i < entity.size(); i++) //check all entities
	//	{
	//		for (size_t j = i + 1; j < entity.size(); j++)
	//		{
	//			/*if (entity[i].has_component()*/
	//		}
	//	}
	//}




	//int testColl() {
	//	AABB box1(Vec2D(0.0f, 0.0f), Vec2D(2.0f,2.0f)); // A box at (0,0) to (2,2)
	//	AABB box2(Vec2D(1.0f, 1.0f), Vec2D(3.0f, 3.0f)); // A box overlapping with box1

	//	float collisionTime;
	//	Collision_System collision_system;
	//	bool isColliding = collision_system.Collision_Intersection_RectRect(box1, Vec2D(0.0f, 0.0f), box2, Vec2D(0.0f, 0.0f), collisionTime);

	//	if (isColliding) {
	//		std::cout << "Collision detected!" << std::endl;
	//	}
	//	else {
	//		std::cout << "No collision." << std::endl;
	//	}

	//	 Move box2 and check again
	//	box2.min.x += 5; // Move box2 away
	//	isColliding = collision_system.Collision_Intersection_RectRect(box1, Vec2D(0, 0), box2, Vec2D(0, 0), collisionTime);

	//	if (isColliding) {
	//		std::cout << "Collision detected!" << std::endl;
	//	}
	//	else {
	//		std::cout << "No collision." << std::endl; // Expect no collision
	//	}
	//	return 0;

	//}




}

