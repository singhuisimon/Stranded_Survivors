/**
 * @file Collision_Syetem.cpp
 * @brief Implements the collsion system.
 * @author Saw Hui Shan
 * @date September 21, 2024
 */
#pragma once

#include "System.h"
#include "../Utility/Vector2D.h" //to get the library from vector2D.h
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"
#include <vector>
#include <iostream>


//namespace lof
//{
//	class ECS_Manager;
//}

namespace lof
{

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
		class ECS_Manager& ecs_manager;
	public:
		Collision_System(ECS_Manager& ecs_manager);

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

		void update(float dt) override;
		//might need: handle collision

		std::string get_type() const override;
	};
}


