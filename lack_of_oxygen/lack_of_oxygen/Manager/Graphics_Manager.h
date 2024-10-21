/**
 * @file Graphics_Manager.h
 * @brief Manages the display and rendering of image data
 * @author Chua Wen Bin Kenny (100%)
 * @date September 18, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_GRAPHICS_MANAGER_H
#define LOF_GRAPHICS_MANAGER_H

#define GLFW_INCLUDE_NONE

// Macros for accessing manager singleton instances
#define GFXM lof::Graphics_Manager::get_instance() 

// Include base Manager class
#include "Manager.h"

// Include other necessary headers
#include "Log_Manager.h"            // For Logging
#include "Input_Manager.h"          // To check for input
#include "Serialization_Manager.h"  // To get screen width and height

// Include Utility headers
#include "../Utility/constant.h"    // To access constants and OpenGL API

// Include standard headers
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <mutex>

namespace lof {

    class Graphics_Manager : public Manager {
    private:

        /**
         * @brief Private constructor to enforce singleton pattern.
         */
        Graphics_Manager();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        Graphics_Manager(const Graphics_Manager&) = delete;

        /**
         * @brief Deleted assignment operator to prevent assignment.
         */
        Graphics_Manager& operator=(const Graphics_Manager&) = delete;

        // Struct of data to create a model
        struct Model {
            GLenum primitive_type;
            GLuint vaoid;
            GLuint draw_cnt;
        };

        // Struct of data to create a shader
        struct ShaderProgram {
            GLuint program_handle = 0;
            GLboolean link_status = GL_FALSE;
        };

        // Storages
        using MODELS = std::map<std::string, Graphics_Manager::Model>;
        using SHADERS = std::vector<ShaderProgram>;
        using TEXTURES = std::map<std::string, GLuint>; 

        // Data members
        static std::unique_ptr<Graphics_Manager> instance;
        static std::once_flag once_flag;
        GLenum render_mode;
        GLboolean is_debug_mode = GL_FALSE;

        // Storage for models and shader programs 
        MODELS model_storage;
        TEXTURES texture_storage; 
        SHADERS shader_program_storage;

    public:

        /**
         * @brief Virtual destructor for Graphics_Manager.
         */
        virtual ~Graphics_Manager();

        /**
         * @brief Gets the singleton instance of Graphics_Manager.
         * @return Reference to the Graphics_Manager instance.
         */
        static Graphics_Manager& get_instance();

        /**
         * @brief Startup the Graphics_Manager and initialize data (serialization).
         * @return 0 if successful, else a negative number.
         */
        int start_up() override;

        /**
         * @brief Shuts down the Graphics_Manager.
         */
        void shut_down() override;

        /**
         * @brief Update the Graphics_Manager and its storage to prepare for rendering.
         */
        void update();

        //////////////////////Shaders & Models-Related functions///////////////////////
        /**
         * @brief Add a shader program into the shader program storage.
         * 
         * @param shaders The filepath to the shaders that are being added.
         * @return True if shader program is added successfully, false otherwise.
         */
        GLboolean add_shader_program(std::vector<std::pair<std::string, std::string>> shaders);

        /**
         * @brief Add a model into the model storage.
         * 
         * @param file_name The filepath to the models that are being added.
         * @return True if the models are added successfully, false otherwise.
         */
        GLboolean add_model(std::string const& file_name);

        /** 
         * @brief Add a texture into the texture storage.
         * 
         * @param file_name The filepath to the textures that are being added.
         * @return True if the textures are added successfully, false otherwise.
         */
        GLboolean add_textures(std::string const& file_name);

        /**
         * @brief Get a reference to the shader program container.
         */
        SHADERS& get_shader_program_storage();

        /**
         * @brief Get a reference to the model container.
         */
        MODELS& get_model_storage();

        /** 
         * @brief Get a reference to the texture container.
         */
        TEXTURES& get_texture_storage(); 

        /**
         * @brief Get a reference to the rendering mode.
         */
        GLenum& get_render_mode();

        /**
         * @brief Get flag of the debug mode.
         */
        GLboolean& get_debug_mode();

        /**
        * @brief Compile the shaders, link the shader objects to create an executable,
                 and ensure the program can work in the current OpenGL state.
        * @param shader_files The data which contains the shader type and its filepath.
        * @param shader The shader program that will be created, compiled, and link.
        * @return True if shader program compile and link successfully, false otherwise.
        */
        GLboolean compile_shader(std::vector<std::pair<GLenum, std::string>> shader_files, ShaderProgram& shader);

        /**
         * @brief Start the shader program
         *
         *.@param shader The shader program that is to be started
         */
        void program_use(ShaderProgram shader);

        /**
         * @brief Free the shader program
         */
        void program_free();

        /**
         * @brief Return the shader program handle
         * 
         * @param shader The shader program that is to return its program handle
         * @return The program handle
         */
        GLuint get_shader_program_handle(ShaderProgram shader) const;
    };

} // namespace lof

#endif // LOF_GRAPHICS_MANAGER_H
