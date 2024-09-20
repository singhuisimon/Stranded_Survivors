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

    ///**
    // * @class Transform2D
    // * @brief Represents the position, rotation, and scale of an entity in 2D space.
    // */
    //class Transform2D : public Component {
    //public:
    //    glm::vec2 position;  ///< Position of the entity in world space.
    //    float rotation;      ///< Rotation of the entity in degrees.
    //    glm::vec2 scale;     ///< Scale of the entity.

    //    /**
    //     * @brief Default constructor initializing position, rotation, and scale.
    //     */
    //    Transform2D()
    //        : position(0.0f, 0.0f),
    //        rotation(0.0f),
    //        scale(1.0f, 1.0f) {}

    //    /**
    //     * @brief Parameterized constructor.
    //     * @param pos Initial position.
    //     * @param rot Initial rotation in degrees.
    //     * @param scl Initial scale.
    //     */
    //    Transform2D(const glm::vec2& pos, float rot, const glm::vec2& scl)
    //        : position(pos), rotation(rot), scale(scl) {}
    //};

    ///**
    // * @class Collider2D
    // * @brief Represents the collision properties of an entity in 2D space using AABB.
    // */
    //class Collider2D : public Component {
    //public:
    //    glm::vec2 size;       ///< Size of the AABB (width and height).

    //    /**
    //     * @brief Constructor for Collider2D.
    //     * @param colliderSize Size of the AABB collider.
    //     */
    //    Collider2D(const glm::vec2& colliderSize = glm::vec2(1.0f, 1.0f))
    //        : size(colliderSize) {}
    //};

} // namespace lof

#endif // LOF_COMPONENT_H