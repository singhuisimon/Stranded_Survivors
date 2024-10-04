/**
 * @file Render_System.h
 * @brief Defines the Render_System class for the ECS.
 * @details Updates the transformation and rendering details of the entity.
 * @author Chua Wen Bin Kenny (100%)
 * @date September 18, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_RENDER_SYSTEM_H
#define LOF_RENDER_SYSTEM_H


#define GLFW_INCLUDE_NONE
// Include file dependencies
#include "System.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Graphics_Manager.h" // For OpenGL API and access to shaders and models

// Include Utility headers
#include "../Utility/constant.h"         // To access constants 

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
        ECS_Manager& ecs_manager;   ///< Reference to the ECS Manager

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
         * @brief Updates the graphical data of the entity and debugging features per frame.
         * @param delta_time The time elapsed since the last update, typically in seconds.
         */
        void update(float delta_time) override;


        /**
         * @brief Renders the entity onto the window based on the entity's components
         */
        void draw();
    };

} // namespace lof

#endif // LOF_RENDER_SYSTEM_H
