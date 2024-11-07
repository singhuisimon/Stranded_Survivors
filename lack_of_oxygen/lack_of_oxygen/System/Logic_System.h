/**
 * @file Logic_System.h
 * @brief Defines the declaration for logic systems.
 * @author Simon Chan (100%)
 * @date November 07, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_LOGIC_SYSTEM_H
#define LOF_LOGIC_SYSTEM_H

#include "System.h"
#include "../Component/Component.h"

namespace lof {
    class Logic_System : public System {
    public:
        /**
         * @brief Constructor for Logic_System.
         */
        Logic_System();

        /**
         * @brief Gets the type identifier for this system.
         * @return String identifying the system type.
         */
        std::string get_type() const override;

        /**
         * @brief Updates the logic state for all relevant entities.
         * @param delta_time Time elapsed since the last update.
         */
        void update(float delta_time) override;

    private:
        /**
         * @brief Updates linear movement for a given entity.
         * @param logic The Logic_Component containing movement parameters.
         * @param transform The Transform2D to update.
         * @param delta_time Time elapsed since the last update.
         */
        void update_linear_movement(Logic_Component& logic, Transform2D& transform, float delta_time);

        /**
         * @brief Updates circular movement for a given entity.
         * @param logic The Logic_Component containing movement parameters.
         * @param transform The Transform2D to update.
         * @param delta_time Time elapsed since the last update.
         */
        void update_circular_movement(Logic_Component& logic, Transform2D& transform, float delta_time);
    };
} // namespace lof
#endif // LOF_LOGIC_SYSTEM_H