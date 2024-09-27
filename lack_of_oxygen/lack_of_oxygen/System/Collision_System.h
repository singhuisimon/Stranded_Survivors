#pragma once
//Collision System
//#include <vector>


#include "System.h"
#include "../Utility/Vector2D.h" //to get the library from vector2D.h
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"
#include <vector>
#include <iostream>

namespace lof
{
	class ECS_Manager;
}

namespace lof
{
	class Collision_Component : public Component
	{
	public:
		float width, height;

		//constructor for collision components 
		Collision_Component(float width = 0.0f, float height = 0.0f)
			: width(width), height(height) {}
	};


	struct AABB {

		Vec2D min;
		Vec2D max;

		//Constructor to initialize minimun and maximun points of AABB
		AABB(const Vec2D& min, const Vec2D& max);

		//create AABB from transform component
		static AABB from_Tranform(const Transform2D& transform, const Collision_Component& collision);
	};

	class Collision_System : public System
	{
	private:
		ECS_Manager& ecs_manager;
	public:
		struct Line_segment
		{
			Vec2D m_pt0; //end of point 0
			Vec2D m_pt1; //end of point 1
			Vec2D m_normal; //normalized outward normal
		};

		struct Circle
		{
			Vec2D m_centre;
			float m_radius;
		};


		//for rectRect
		bool Collision_Intersection_RectRect(const AABB& aabb1,
											const Vec2D& vel1,
											const AABB& aabb2,
											const Vec2D& vel2,
											float& firstTimeOfCollision);

		//function to check if a point is within a box
		bool is_Intersept_Box(float pos_box_x, float pos_box_y, float width_box, float height_boxx, int mouseX, int mouseY);

		//check collision within grid cell
		/*void Check_Collision_In_Grid_Cell(const std::vector<Entity>& entities, const AABB& gridCell);

		void Update_Grid(std::vector<Entity>& entites);*/


		////to check the object that is collide 
		//bool Check_Collision(const std::vector<Entity>& entity,
		//	const std::vector<Position_Component>& pos,
		//	const std::vector<Velocity_Component>& vel);
		
		/*void Collision_Update(std::vector<Entity>& entities, float dt);*/
		void Collision_Update(float dt);
		//might need: handle collision
	};





}


