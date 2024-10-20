/**
 * @file Movement_System.cpp
 * @brief Implements the physics (gravity, jumping) and movement for the game entities
 * @author Wai Lwin Thit (100%)
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */


#pragma once

#include "System.h"
namespace lof {

    /**
     * @class Movement_System
     * @brief System responsible for updating entities' positions based on their velocities.
     */
    class Movement_System : public System {
    public:
        /**
         * @brief Constructor for Movement_System.
         * @param ecs_manager Reference to the ECS_Manager.
         */
        Movement_System(class ECS_Manager& ecs_manager);

        /**
         * @brief Updates the system.
         * @param delta_time The time elapsed since the last update.
         */
        void update(float delta_time) override;

        /**
         * @brief Returns the system's type.
         * @return The string "Movement_System".
         */
        std::string get_type() const override;

    private:
        class ECS_Manager& ecs;
    };

} // namespace lof
