/**
 * @file Object_Moving_Script.h
 * @brief Define the object moving script class.
 * @author Amanda Leow Boon Suan (80%), Simon Chan (20%)
 * @date February 2, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#ifndef LOF_OBJECT_MOVING_SCRIPT_H
#define LOF_OBJECT_MOVING_SCRIPT_H

#include <memory>
#include <unordered_map>
#include <functional>
#include "../Scripts/Script.h"
#include "../Component/Component.h"

namespace lof {
	/**
	 * @class Object_Moving_Script
	 * @brief Defines the object moving script class.
	 */
	class Object_Moving_Script : public Script, public std::enable_shared_from_this<Object_Moving_Script> {

    public:
        /**
         * @struct MovementData
         * @brief Defines the object moving script class.
         */
        struct MovementData {
            int movement_pattern = 0;
            float movement_speed = 0.0;
            float movement_range = 0.0;
            Vec2D origin_pos = Vec2D();
            bool reverse_direction = false;
            bool rotate_with_motion = false;
            float timer = 0.0f;
            int pattern = 0;
        };

    public:

		/**
		 * @brief Register the script and function as well as implement the function
		 */
        void register_script() override;

		/**
		 * @brief Default constructor
		 */
        Object_Moving_Script();

        /**
		 * @brief Destructor
         */
        ~Object_Moving_Script() override;

        /**
        *
        */
		std::string get_type() const override;

        /**
		 * @brief Cleans up the unordered map of entity data.
         */
        void cleanup();

    private:

        const std::string script_name = "object_moving_script";

        //mutable std::mutex entity_data_mutex;
        std::unordered_map<EntityID, MovementData> entity_data;

        /**
         * @brief Add entity data to the unordered mao
		 * @param id The entity id
		 * @param data The MovementData to be added
         */
        void add_entity_data(EntityID id, const MovementData& data);

        /**
		 * @brief Retrieve the entity data from the unordered map
		 * @param id The entity id
		 * @param out_data The MovementData to be retrieved
		 * @return True if the entity data is found, false otherwise.
         */
        bool get_entity_data(EntityID id, MovementData& out_data) const;

        /**
		 * @brief Remove entity data from the unordered map
		 * @param id The entity id
         */
        void remove_entity_data(EntityID id);

        /**
        * @brief Updates linear movement for a given entity.
        * @param logic The Logic_Component containing movement parameters.
        * @param transform The Transform2D to update.
        * @param delta_time Time elapsed since the last update.
        */
        void update_linear_movement(MovementData& data, Transform2D& transform_comp);

        /**
        * @brief Updates circular movement for a given entity.
        * @param logic The Logic_Component containing movement parameters.
        * @param transform The Transform2D to update.
        * @param delta_time Time elapsed since the last update.
        */
        void update_circular_movement(MovementData& data, Transform2D& transform_comp);
	};
}

#endif