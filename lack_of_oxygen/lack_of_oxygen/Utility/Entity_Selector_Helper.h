#pragma once
#ifndef ENTITY_SELECTOR_HELPER_H
#define ENTITY_SELECTOR_HELPER_H

#include "../Main/Main.h" // for extern window
#include <iostream>
#include "../Utility/Vector2D.h"

namespace lof {

#define ESS lof::Entity_Selector_Helper::get_instance()
#if 1
	struct EntityInfo {
		EntityID selectedEntity;
		bool isSelected; // Flag to indicate if an entity is selected
		ImVec2 mousePos;  // Store the mouse position 
		Vec2D entitypos;
	};
#endif
	class Entity_Selector_Helper
	{
	public:

		static Entity_Selector_Helper& get_instance();

		static EntityInfo& get_selected_entity_info();

		void Check_Selected_Entity();
		
		Entity_Selector_Helper() = default;
		
		void Update_Selected_Entity_Info(EntityID entityID, float entityX, float entityY, float entityWidth, float entityHeight);

		
		bool Mouse_Over_AABB(float box_x, float box_y, float width, float height, int X, int mouseY);
		
		Vec2D Get_World_MousePos();


	private:


		static EntityInfo g_selected_entity_info;




		static std::unique_ptr<Entity_Selector_Helper> instance;

		static std::once_flag once_flag;


	};
}



#endif