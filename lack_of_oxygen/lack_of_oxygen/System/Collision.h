#pragma once
//Collision System
//#include <vector>


#include "../Utility/Vector2D.h" //to get the library from vector2D.h
#include <vector>
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../Manager/ECS_Manager.h"


namespace lof
{
	class Collision_Component
	{
	public:
		float width, height;

		//constructor for collision components 
		Collision_Component(float width, float height);
	};

	//class for transform component
	class Transform_Component
	{
	public:
		Vec2D pos; //position 
		float m_width, m_height; //size
		float m_rotation; //rotation

		//constructors for transfrom 
		Transform_Component(float x, float y, float width, float height, float rotation);


	};

	struct AABB {

		Vec2D min;
		Vec2D max;

		//Constructor to initialize minimun and maximun points
		AABB(const Vec2D& min, const Vec2D& max);

		//create AACC from transform component
		static AABB from_Tranform(const Transform_Component& transform);
	};

	class Collision_System
	{
	public:
		bool Collision_Intersection_RectRect(const AABB& aabb1,
			const Vec2D& vel1,
			const AABB& aabb2,
			const Vec2D& vel2,
			float& firstTimeOfCollision);

		//function to check if a point is within a box
		bool is_Intersept_Box(float box_x, float box_y, float width, float height, int mouseX, int mouseY);


		////to check the object that is collide 
		//bool Check_Collision(const std::vector<Entity>& entity,
		//	const std::vector<Position_Component>& pos,
		//	const std::vector<Velocity_Component>& vel);
		
		void Collision_Update(std::vector<Entity>& entities, float dt);

		
	};



}


