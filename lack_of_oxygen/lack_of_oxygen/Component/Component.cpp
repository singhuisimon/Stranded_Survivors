/**
 * @file Component.cpp
 * @brief Implements the specific component classes.
 * @details This file contains the implementation of the Position_Component and Velocity_Component classes.
 * @author Simon Chan
 * @date September 15, 2024
 */

// Include header file
#include "Component.h"

namespace lof {

    /**
     * @brief Constructor for Position_Component.
     * @param x Initial x-coordinate of the entity's position.
     * @param y Initial y-coordinate of the entity's position.
     */
    Position_Component::Position_Component(float x, float y) : x(x), y(y) {}

    /**
     * @brief Constructor for Velocity_Component.
     * @param vx Initial velocity along the X-axis.
     * @param vy Initial velocity along the Y-axis.
     */
    Velocity_Component::Velocity_Component(float vx, float vy) : vx(vx), vy(vy) {}



} // namespace lof