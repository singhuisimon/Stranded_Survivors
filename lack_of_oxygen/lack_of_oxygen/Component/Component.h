/**
 * @file Component.h
 * @brief Defines the base Component class and specific component types for the Entity Component System (ECS).
 * @details This file establishes the foundation for creating various game object properties and behaviors.
 *          It includes the base Component class and derived classes for position and velocity components.
 * @author Simon Chan
 * @date September 15, 2024
 */

#ifndef LOF_COMPONENT_H
#define LOF_COMPONENT_H

// Include standard headers
#include <cstdint>

namespace lof {

    /**
     * @typedef ComponentID
     * @brief Alias for the data type used to represent component identifiers.
     */
    using ComponentID = std::uint8_t;

    /**
     * @class Component
     * @brief Base class for all components in the ECS.
     */
    class Component {
    public:
        /**
         * @brief Virtual destructor for the Component class.
         */
        virtual ~Component() = default;
    };

    /**
     * @class Position_Component
     * @brief Component representing an entity's position.
     */
    class Position_Component : public Component {
    public:
        float x; ///< X-coordinate
        float y; ///< Y-coordinate

        /**
         * @brief Constructor for Position_Component.
         * @param x Initial x-coordinate.
         * @param y Initial y-coordinate.
         */
        Position_Component(float x = 0.0f, float y = 0.0f);
    };

    /**
     * @class Velocity_Component
     * @brief Component representing an entity's velocity.
     */
    class Velocity_Component : public Component {
    public:
        float vx; ///< Velocity along the X-axis
        float vy; ///< Velocity along the Y-axis

        /**
         * @brief Constructor for Velocity_Component.
         * @param vx Initial velocity along the X-axis.
         * @param vy Initial velocity along the Y-axis.
         */
        Velocity_Component(float vx = 0.0f, float vy = 0.0f);
    };

} // namespace lof

#endif // LOF_COMPONENT_H