/**
 * @file Component.h
 * @brief Defines the base Component class and specific component types for the Entity Component System (ECS).
 * @details This file establishes the foundation for creating various game object properties and behaviors.
 *          It includes the base Component class and derived classes for position, velocity, and model components.
 * @date September 15, 2024
 */

#ifndef LOF_COMPONENT_H
#define LOF_COMPONENT_H

 // Include standard headers
#include <cstdint>
#include <string>
#include <memory>

// Include Utility headers
#include "../Utility/Vector2D.h"
#include "../Utility/Model.h" 

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
     * @class Model_Component
     * @brief Component representing the model associated with an entity.
     */
    class Model_Component : public Component {
    public:
        std::string model_name;                          ///< Name of the model (e.g., "square").
        std::shared_ptr<Model> model_ptr;                ///< Pointer to the loaded Model.

        /**
         * @brief Constructor for Model_Component.
         * @param name Name identifier for the model.
         * @param model Pointer to the loaded Model.
         */
        Model_Component(const std::string& name = "", std::shared_ptr<Model> model = nullptr)
            : model_name(name), model_ptr(model) {}
    };

    /**
     * @class Transform2D
     * @brief Represents the position, rotation, and scale of an entity in 2D space.
     */
    class Transform2D : public Component {
    public:
        Vec2D position;  ///< Position of the entity in world space.
        float rotation;  ///< Rotation of the entity in degrees.
        Vec2D scale;     ///< Scale of the entity.

        /**
         * @brief Default constructor initializing position, rotation, and scale.
         */
        Transform2D()
            : position(0.0f, 0.0f), rotation(0.0f), scale(1.0f, 1.0f) {}

        /**
         * @brief Parameterized constructor.
         * @param pos Initial position.
         * @param rot Initial rotation in degrees.
         * @param scl Initial scale.
         */
        Transform2D(const Vec2D& pos, float rot, const Vec2D& scl)
            : position(pos), rotation(rot), scale(scl) {}
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

        /**
           * @brief Constructor for Physics_Component
           * @   Sets up the data values required for physics component
           */
        Physics_Component(Vec2D gravity = Vec2D(0, -9.8f),
            float damping_factor = 0.90f,
            float max_velocity = 1000.0f,
            float mass = 1.0f,
            bool is_static = false)

            : gravity(gravity),
            damping_factor(damping_factor),
            max_velocity(max_velocity),
            accumulated_force(Vec2D(0, 0)),
            mass(mass),
            inv_mass((mass > 0.0f) ? 1.0f / mass : 0.0f),
            is_static(is_static) {}

        //Apply force
        void apply_force(const Vec2D& force) {
            accumulated_force += force;
        }

        //set the mass of the object
        void set_mass(float m) {
            mass = m;
            inv_mass = (m > 0.0f) ? 1.0f / m : 0.0f;
        }

    };
    /** // FOR TESTING FIRST (VALUES WILL BE READ FROM A FILE IN THE FUTURE)
    * @class Graphics_Component
    * @brief Component representing an entity's graphical data.
    */
    class Graphics_Component : public Component {
    public:
        // Organic members of Graphics_Component
        glm::mat3 mdl_to_ndc_xform;
        GLuint mdl_ref, shd_ref;
        // texture/sprite member in the future

        /**
         * @brief Constructor for Graphics_Component.
         * @param mdl_to_ndc_xform Model-to-world-to-NDC transformation.
         * @param mdl_ref Reference to model
         * @param shd_ref Reference to shader
         */

        Graphics_Component(GLuint model, GLuint shader,
            glm::mat3 xform) : mdl_ref(model), shd_ref(shader), mdl_to_ndc_xform(xform) {}

    };

} // namespace lof

#endif // LOF_COMPONENT_H
