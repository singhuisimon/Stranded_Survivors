/**
 * @file Render_System.h
 * @brief Defines the Render_System class for the ECS.
 * @details Updates the transformation and rendering details of the entity.
 * @author Chua Wen Bin Kenny
 * @date September 18, 2024
 */

#ifndef LOF_RENDER_SYSTEM_H
#define LOF_RENDER_SYSTEM_H


#define GLFW_INCLUDE_NONE
 // Include file dependencies
#include "System.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Graphics_Manager.h" // (For OpenGL API and access to shaders and models)

// Include stb_image for texture mapping (KIV)

 // Forward declaration
namespace lof {
    class ECS_Manager;
}

namespace lof {

    /**
     * @class System
     * @brief Abstract base class for all systems in the ECS.
     */
    class Render_System : public System {
    private:
        ECS_Manager& ecs_manager; ///< Reference to the ECS Manager
        //Graphics_Manager& gfx_manager; ///< Reference to the Graphics Manager

    public:
        /**
         * @brief Constructor for Movement_System.
         * @param manager Reference to the ECS_Manager.
         */
        Render_System(ECS_Manager& ecs_manager);

        /**
         * @brief Returns the system's type.
         * @return The string "Render_System".
         */
        std::string get_type() const override;

        /**
         * @brief Updates the model-to-world-to-NDC transformation matrix of the component per frame.
         * @param delta_time The time elapsed since the last update, typically in seconds.
         */
        void update(float delta_time) override;


        /**
         * @brief Renders the entity onto the window based on the graphics components
         */
        void draw();
    };

} // namespace lof

#endif // LOF_RENDER_SYSTEM_H
