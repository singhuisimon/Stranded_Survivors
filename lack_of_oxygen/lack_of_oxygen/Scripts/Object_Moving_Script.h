/**
 * @file Object_Moving_Script.h
 * @brief Declare gameplay logic function for object moving.
 * @author Simon Chan (100%), Amanda Leow Boon Suan
 * @date February 2, 2025
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#ifndef LOF_OBJECT_MOVING_SCRIPT_H
#define LOF_OBJECT_MOVING_SCRIPT_H

#include "../Scripts/Script.h"
#include "../Component/Logic_Components.h"

namespace lof {
	class Object_Moving_Script : public Script {

    public:
        struct EntityData {
            std::string movement_pattern;
            float movement_speed;
            float movement_range;
            Vec2D origin_pos;
            bool reverse_direction;
            bool rotate_with_motion;
            float timer = 0.0f;
            int pattern;
        };

    public:

        static void register_script();

        Object_Moving_Script() = default;

        void init_get_movement_data(Entity* entity);
        void update_obj_movement(Entity* entity);

    private:

        const std::string script_name = "object_moving_script";

        std::unordered_map<EntityID, EntityData> entity_data;

        /**
        * @brief Updates linear movement for a given entity.
        * @param logic The Logic_Component containing movement parameters.
        * @param transform The Transform2D to update.
        * @param delta_time Time elapsed since the last update.
        */
        void update_linear_movement(EntityData& data, Transform2D& transform_comp);

        /**
        * @brief Updates circular movement for a given entity.
        * @param logic The Logic_Component containing movement parameters.
        * @param transform The Transform2D to update.
        * @param delta_time Time elapsed since the last update.
        */
        void update_circular_movement(EntityData& data, Transform2D& transform_comp);
	};
}

#endif