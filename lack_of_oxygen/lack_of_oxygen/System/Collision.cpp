#include "Collision.h"
#include "../Component/Component.h"

namespace lof
{
	//default constructor -  initialize the width height 
	Collision_Component::Collision_Component(float width, float height)
		: width(width), height(height) {}

	//constructor for Transform_Component
	Transform_Component::Transform_Component(float x, float y, float width, float height, float rotation)
		: pos(x, y), m_width(width), m_height(height), m_rotation(rotation) {}

	//constructor for AABB
	AABB::AABB(const Vec2D& min, const Vec2D& max)
		: min(min), max(max) {}

	AABB AABB::from_Tranform(const Transform_Component& transform)
	{
		Vec2D min;
		Vec2D max;

		//get the 
		min.x = transform.pos.x - (transform.m_width / 2.0f);
		min.y = transform.pos.y - (transform.m_height / 2.0f);
		max.x = transform.pos.x - (transform.m_width / 2.0f);
		max.y = transform.pos.x - (transform.m_height / 2.0f);

		return AABB(min, max);
		//calclutate the min and max point based on position and size
	}

	bool Collision_System::Collision_Intersection_RectRect(const AABB& aabb1,
		const Vec2D& vel1,
		const AABB& aabb2,
		const Vec2D& vel2,
		float& firstTimeOfCollision)
	{

		if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x) //check for A.max < B.min or A.min > B.min for x axis
		{
			return false; //no intersection
		}

		if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) // check for same thing but for y-axis
		{
			return false; //no intersection
		}

		float Vb_x; //initialize Vb for x axis 
		Vb_x = vel2.x - vel1.x;
		float Vb_y; //initialize Vb for y axis
		Vb_y = vel2.y - vel1.y;

		float dFirst_x; //initialize for dFirst for x-axis
		dFirst_x = aabb1.max.x - aabb2.min.x;

		float dFirst_y; // initialize for dFrist for y axis
		dFirst_y = aabb1.max.y - aabb2.min.y;

		float dLast_x; //initialize for dLast for x axis
		dLast_x = aabb1.min.x - aabb2.max.x;

		float dLast_y; //initialize for dLast for y axis
		dLast_y = aabb1.min.y - aabb2.max.y;

		float tFirst = firstTimeOfCollision = 0.0f; //Initialise t first
		float tLast = 1.0f; //initialize tLast and assume 1 as the time step (g_dt)

		//for x-axis
		if (Vb_x < 0)
		{
			//case 1 for x-axis
			if (aabb1.min.x > aabb2.max.x)
			{
				return 0;
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
				return 0;
			}

		}
		else if (Vb_x == 0) {
			//case 5;
			if (aabb1.max.x < aabb2.min.x)
			{
				return 0;
			}
			else if (aabb1.min.x > aabb2.max.x)
			{
				return 0;
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
				return 0;
			}
		}
		else if (Vb_y == 0) {
			//case 5;
			if (aabb1.max.y < aabb2.min.y)
			{
				return 0;
			}
			else if (aabb1.min.y > aabb2.max.y)
			{
				return 0;
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

