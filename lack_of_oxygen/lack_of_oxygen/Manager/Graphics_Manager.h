/**
 * @file Graphics_Manager.h
 * @brief Manages the display and rendering of image data
 * @author Chua Wen Bin Kenny
 * @date September 18, 2024
 */

#ifndef LOF_GRAPHICS_MANAGER_H
#define LOF_GRAPHICS_MANAGER_H

#define GLFW_INCLUDE_NONE

 // Include base Manager class
#include "Manager.h"

 // Include other necessary headers
#include "Log_Manager.h"    // For Logging
#include "Input_Manager.h"  // To check for input

 // Include OpenGL headers
#define GLFW_INCLUDE_NONE
#include "../Glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm-0.9.9.8/glm/glm.hpp>
#include <glm-0.9.9.8/glm/gtc/type_ptr.hpp>

 // Include standard headers
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <list>
#include <map>

namespace lof {

#define GFXM lof::Graphics_Manager::get_instance()

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
            GLuint primitive_cnt;
            GLuint vaoid;
            GLuint draw_cnt;
        };

        // Struct of data to create a shader
        struct ShaderProgram {
            GLuint program_handle = 0;
            GLboolean link_status = GL_FALSE;
        };

        //using MODELS = std::vector<Graphics_Manager::Model>;
        using MODELS = std::map<std::string, Graphics_Manager::Model>;
        using SHADERS = std::vector<ShaderProgram>;

        // Data members
        static std::unique_ptr<Graphics_Manager> instance;
        static std::once_flag once_flag;
        GLenum render_mode;
        GLboolean is_debug_mode = GL_FALSE;

        // Storage for models and shader programs (Technically there is only one shader program based on our needs)
        MODELS model_storage;
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

        ///////////////////// Shaders & Models-Related functions///////////////////////
        /**
         * @brief Add a shader program into the shader program storage.
         */
        GLboolean add_shader_program(std::vector<std::pair<std::string, std::string>> shaders);

        /**
         * @brief Add a model into the model storage.
         */
        GLboolean add_model(std::string const& file_name);

        /**
         * @brief Get a reference to the shader program container.
         */
        SHADERS& get_shader_program_storage();

        /**
         * @brief Get a reference to the model container.
         */
        MODELS& get_model_storage();

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
        */
        GLboolean compile_shader(std::vector<std::pair<GLenum, std::string>> shader_files, ShaderProgram& shader);

        void program_use(ShaderProgram shader);

        void program_free();

        GLuint get_shader_program_handle(ShaderProgram shader) const;

    };

} // namespace lof

#endif // LOF_GRAPHICS_MANAGER_H
