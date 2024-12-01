/**
 * @file Entity_Selector_Helper.h
 * @brief Implements the entity selector helper to get the entity that the mouse point to
 * @author Saw Hui Shan (100%)
 * @date December 1, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.

 */


#pragma once
#ifndef ENTITY_SELECTOR_HELPER_H
#define ENTITY_SELECTOR_HELPER_H

#include <iostream>

#include "../Main/Main.h" // for extern window
#include "../Utility/Vector2D.h"

namespace lof {
// For accessing Entity Selector instances
#define ESS lof::Entity_Selector_Helper::get_instance()

	/**
	* @struct EntityInfo
	* @brief Stores information about the currently selected entity.
	*/
	struct EntityInfo {
		EntityID selectedEntity = static_cast<uint32_t>(-1);
		bool isSelected = false; // Flag to indicate if an entity is selected
		ImVec2 mousePos;  // Store the mouse position 
		Vec2D entitypos; // get entity position
	};

	/**
	* @class Entity_Selector_Helper
	*/
	class Entity_Selector_Helper
	{
	public:

		/**
		* @brief Get the singleton instance of the Entity_Selector_Helper.
		*
		* @return Reference to the singleton instance.
		*/
		static Entity_Selector_Helper& get_instance();

		/**
		* @brief Get information about the currently selected entity.
		*
		* @return Reference selected entity's details in EntityInfo struct.
		*/
		static EntityInfo& get_selected_entity_info();

		/**
		* @brief Check and update the currently selected entity based on mouse position.
		*/
		void Check_Selected_Entity();
		
		/**
		* @brief Default constructor.
		*/
		Entity_Selector_Helper() = default;
		
		/**
		* @brief Update the selected entity's information.
		*
		* @param entityID
		*    The ID of the selected entity
		* @param entityX
		*    x coord of the entity's position
		* @param entityY
		*    y coord of the entity's position
		* @param entityWidth
		*    Width of the entity
		* @param entityHeight
		*    Height of the entity
		*/
		void Update_Selected_Entity_Info(EntityID entityID, float entityX, float entityY, float entityWidth, float entityHeight);

		/**
		* @brief Check if the mouse pointer is over an axis-aligned bounding box (AABB).
		*
		* @param box_x
		*    x coord of the top-left corner of the AABB.
		* @param box_y
		*    y coord of the top-left corner of the AABB.
		* @param width
		*    Width of the AABB.
		* @param height
		*    Height of the AABB.
		* @param X
		*    X-coordinate of the mouse position.
		* @param mouseY
		*    Y-coordinate of the mouse position.
		*
		* @return
		*    True if the mouse pointer is over the AABB, false otherwise.
		*/
		bool Mouse_Over_AABB(float box_x, float box_y, float width, float height, float X, float mouseY);
		
		/**
		* @brief Get the current mouse position in world coordinates.
		*
		* @return World mouse position.
		*/
		Vec2D Get_World_MousePos();


	private:


		static EntityInfo g_selected_entity_info; // store info of current selected entity

		static std::unique_ptr<Entity_Selector_Helper> instance;

		static std::once_flag once_flag;
	};
}

#endif