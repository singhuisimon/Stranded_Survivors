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

namespace lof {

    /**
     * @class Render_System
     * @brief System responsible for rendering entities.
     */
    class Render_System : public System {
    public:
        /**
         * @brief Constructor for Render_System.
         * Initializes the system's signature.
         */
        Render_System();

        /**
         * @brief Returns the system's type as a string.
         * @return The system's type name.
         */
        std::string get_type() const override;

        /**
         * @brief Updates the graphical data of entities and handles rendering per frame.
         * @param delta_time The time elapsed since the last update, typically in seconds.
         */
        void update(float delta_time) override;

    private:
        /**
         * @brief Renders entities onto the window based on their components.
         */
        void draw();
    };

} // namespace lof

#endif // LOF_RENDER_SYSTEM_H
