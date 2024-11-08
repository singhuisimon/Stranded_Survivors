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
#ifndef LOF_MOVEMENT_SYSTEM_H
#define LOF_MOVEMENT_SYSTEM_H

#include "System.h"
#include "../Manager/ECS_Manager.h"
#include <vector>

namespace lof {

    /**
     * @class Movement_System
     * @brief System responsible for updating entities' positions based on their velocities and physics.
     *
     * This system processes entities with Transform2D, Velocity_Component, and Physics_Component,
     * applying physics-based calculations to update their positions, velocities, and other related
     * properties. It manages forces like gravity and jumping.
     */
    class Movement_System : public System {
    public:
        /**
         * @brief Constructor for Movement_System.
         * Initializes the system's signature.
         */
        Movement_System();
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
        /**
         * @brief Integrates physics calculations for movement, applying forces and updating positions.
         * @param delta_time The time increment for updating entity positions and velocities.
         */
        void integrate(float deltatime);

    };
   
} // namespace lof

#endif // LOF_MOVEMENT_SYSTEM_H
