/**
 * @file Movement_System.h
 * @brief Defines the Movement_System class for handling entity movement.
 * @date September 15, 2024
 */

#pragma once

#include "System.h"
//#include "../Manager/Input_Manager.h"

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
       // Input_Manager& input_manager; // Add a reference to Input_Manager
    };

} // namespace lof
