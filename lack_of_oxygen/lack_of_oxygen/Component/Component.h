/**
 * @file Component.h
 * @brief Defines the base Component class and specific component types for the Entity Component System (ECS).
 * @author Simon Chan
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_COMPONENT_H
#define LOF_COMPONENT_H

// Include standard headers
#include <cstdint>
#include <string>
#include <memory>

// Include Utility headers
#include "../Utility/Vector2D.h"
#include "../Utility/Constant.h"

// FOR TESTING 
#include "../Glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm-0.9.9.8/glm/glm.hpp>
#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp>

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
     * @class Transform2D
     * @brief Represents the position, rotation, and scale of an entity in 2D space.
     */
    class Transform2D : public Component {
    public:
        Vec2D position;     ///< Position of the entity in world space.
        Vec2D orientation;  ///< Orientation of the entity in degrees.
        Vec2D scale;        ///< Scale of the entity.

        /**
         * @brief Default constructor initializing position, rotation, and scale.
         */
        Transform2D()
            : position(0.0f, 0.0f), orientation(0.0f, 0.0f), scale(1.0f, 1.0f) {}

        /**
         * @brief Parameterized constructor.
         * @param pos Initial position.
         * @param rot Initial rotation in degrees.
         * @param scl Initial scale.
         */
        Transform2D(const Vec2D& pos, Vec2D& ori, const Vec2D& scl)
            : position(pos), orientation(ori), scale(scl) {}
    };


    /**
     * @class Velocity_Component
     * @brief Component representing an entity's velocity.
     */
    class Velocity_Component : public Component {
    public:
        Vec2D velocity; ///< Velocity of the entity.

        /**
         * @brief Constructor for Velocity_Component.
         * @param vx Initial velocity along the X-axis.
         * @param vy Initial velocity along the Y-axis.
         */
        Velocity_Component(float vx = 0.0f, float vy = 0.0f)
            : velocity(vx, vy) {}
    };

    /**
    * @class Physics_Component
    * @brief Component representing global physics properties
    */
    class Physics_Component : public Component {
    public:
        Vec2D gravity;
        float damping_factor;
        float max_velocity;
        Vec2D accumulated_force; //to accumulate forces applied to the entity.

        float mass; //mass of entity
        float inv_mass;
        bool is_static; //to check if the entity is static or not
        bool is_moveable; //to check if the entity is moveable or not

        bool is_grounded; //to indicate whether the entity is on the ground
        bool is_jumping; //to indicate whether the entity is jumping
        float jump_force; //the force applied during a jump


    public:
        /**
     * @brief Constructor for Physics_Component.
     *
     * Initializes the physics properties of the entity, including gravity, damping factor, max velocity,
     * mass, static state, movability, and jump force.
     *
     * @param gravity The gravity vector affecting the entity. Default is (0, DEFAULT_GRAVITY).
     * @param damping_factor The factor to dampen velocity over time. Default is DEFAULT_DAMPING_FACTOR.
     * @param max_velocity The maximum velocity of the entity. Default is DEFAULT_MAX_VELOCITY.
     * @param mass The mass of the entity. Default is 1.0f.
     * @param is_static Indicates if the entity is static. Default is false.
     * @param is_moveable Indicates if the entity can be moved. Default is false.
     * @param jump_force The force applied during a jump. Default is DEFAULT_JUMP_FORCE.
     */
        Physics_Component(Vec2D gravity = Vec2D(0, DEFAULT_GRAVITY),
            float damping_factor = DEFAULT_DAMPING_FACTOR,
            float max_velocity = DEFAULT_MAX_VELOCITY,
            float mass = 1.0f,
            bool is_static = false, 
            bool is_moveable = false,
            float jump_force = DEFAULT_JUMP_FORCE ) //adjust later

            : gravity(gravity),
            damping_factor(damping_factor),
            max_velocity(max_velocity),
            accumulated_force(Vec2D(0, 0)),
            mass(mass),
            inv_mass((mass > 0.0f) ? 1.0f / mass : 0.0f),
            is_static(is_static),
            is_moveable(is_moveable),
            is_grounded(false),
            is_jumping(false),
            jump_force(jump_force) {}

        /**
         * @brief Applies a force to the entity.
         *
         * Adds the given force to the accumulated forces acting on the entity.
         *
         * @param force The force vector to be applied.
         */

        void apply_force(const Vec2D& force) {
            accumulated_force += force;
        }
        /**
         * @brief Resets all accumulated forces acting on the entity.
         *
         * Sets the accumulated force to zero, preparing the entity for the next physics update.
         */
        void reset_forces() {
            accumulated_force = Vec2D(0, 0);
        }

           /**
            * @brief Sets the mass of the entity.
            *
            * Adjusts the mass and its inverse. If the mass is zero or negative, the inverse mass is set to zero.
            *
            * @param m The new mass of the entity.
            */        
        void set_mass(float m) {
            mass = m;
            inv_mass = (m > 0.0f) ? 1.0f / m : 0.0f;
        }
    };
    /**
    * @class Graphics_Component
    * @brief Component representing an entity's graphical data.
    */
    class Graphics_Component : public Component {
    public:
        std::string model_name;
        glm::vec3 color;
        GLuint shd_ref;
        glm::mat3 mdl_to_ndc_xform;

        // Default constructor
        Graphics_Component()
            : model_name(DEFAULT_MODEL_NAME), color(DEFAULT_COLOR), shd_ref(DEFAULT_SHADER_REF), mdl_to_ndc_xform(DEFAULT_MDL_TO_NDC_MAT) {}

        /**
         * @brief Constructor for Graphics_Component.
         * @param mdl_to_ndc_xform Model-to-world-to-NDC transformation.
         * @param mdl_ref Reference to model
         * @param shd_ref Reference to shader
         */

        Graphics_Component(std::string mdl_name, glm::vec3 clr, GLuint shader,
            glm::mat3 xform) : model_name(mdl_name), color(clr), shd_ref(shader), mdl_to_ndc_xform(xform) {}

    }; 

    /**
    * @class Collision_Component
    * @brief Component representing an entity's component data.
    */
    class Collision_Component : public Component
    {
    public:
        float width, height;

        //constructor for collision components 
        Collision_Component(float width = 0.0f, float height = 0.0f)
            : width(width), height(height) {}
    };
} // namespace lof

#endif // LOF_COMPONENT_H
