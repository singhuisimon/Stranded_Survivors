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
#include "Assets_Manager.h"         // For assets management

// Include Utility headers
#include "../Utility/constant.h"    // To access constants and OpenGL API

// Include standard headers
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <unordered_map>
#include <mutex>

namespace lof {

    class Graphics_Manager : public Manager {
        friend class Assets_Manager;

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

        // Struct of a frame for animation
        struct Frame {
            unsigned int frame_number;
            float time_delay;
        };

        // Struct of an animation
        struct Animation {
            std::vector<Frame> frames;  // Collection of frames time_delay 
            std::string texture_name;
            unsigned int curr_frame_index{ DEFAULT_FRAME_INDEX };
            float frame_elapsed_time{ DEFAULT_FRAME_TIME_ELAPSED };        // Time elapsed for current frame
            bool is_updated{ false };
        };

        // Struct of a camera
        struct Camera2D {
            GLfloat pos_x, pos_y;
            glm::mat3 view_xform, camwin_to_ndc_xform, world_to_ndc_xform;

            // Toggle for free camera mode
            GLboolean is_free_cam;

            Camera2D() : pos_x(DEFAULT_CAMERA_POS_X), pos_y(DEFAULT_CAMERA_POS_Y), view_xform(0),
                        camwin_to_ndc_xform(0), world_to_ndc_xform(0), is_free_cam(GL_FALSE) {};
        };

        // Struct of a character
        struct Character {
            unsigned int TextureID; // ID handle of the glyph texture
            glm::ivec2   Size;      // Size of glyph
            glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
            unsigned int Advance;   // Horizontal offset to advance to next glyph
        };

        // Struct of a font
        struct Font {
            GLuint vaoid{ 0 };
            GLuint vboid{ 0 };
            std::map<GLchar, Character> characters; // Store the full range of characters
        };

        // Storages
        using MODELS = std::map<std::string, Graphics_Manager::Model>;
        using TEXTURES = std::map<std::string, GLuint>;
        using ANIMATIONS = std::unordered_map<std::string, Animation>;
        using FONTS = std::map<std::string, Font>;

        MODELS model_storage;
        TEXTURES texture_storage;
        ANIMATIONS  animation_storage;
        FONTS font_storage;

        // Data members
        static std::unique_ptr<Graphics_Manager> instance;
        static std::once_flag once_flag;
        GLenum render_mode;
        GLboolean is_debug_mode = GL_FALSE;
        Camera2D camera{}; 

        // Flags to prevent scaling and rotation buttons from conflicting
        int scale_flag = 0;
        int rotation_flag = 0;

        // Flag for player direction animation
        int player_direction = FACE_LEFT;
        int is_moving = 0;
        int is_mining = 0;

        // Texture for imgui level editor viewport
        GLuint imgui_fbo{ 0 }, imgui_tex{ 0 };
        int editor_mode = 0;

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
         * @brief Add a model into the model storage.
         *
         * @param file_name The filepath to the models that are being added.
         * @return True if the models are added successfully, false otherwise.
         */
        GLboolean add_model(std::string const& file_name);

        /**
         * @brief Add a texture into the texture storage.
         *
         * @param texture_name The name of the textures that are being added.
         * @return True if the texture is added successfully, false otherwise.
         */
        GLboolean load_texture(std::string const& texture_name);

        /**
         * @brief Add animations into the animation storage.
         *
         * @param file_name The filepath to the animations that are being added.
         * @return True if the animations are added successfully, false otherwise.
         */
        GLboolean add_animations(std::string const& file_name);

        /**
         * @brief Add fonts into the fonts storage.
         *
         * @param file_name The filepath to the fonts that are being added.
         * @return True if the fonts are added successfully, false otherwise.
         */
        GLboolean add_fonts(std::string const& file_name);

        /**
         * @brief Get a reference to the model container.
         */
        MODELS& get_model_storage();

        /**
         * @brief Get a reference to the texture container.
         */
        TEXTURES& get_texture_storage();

        /**
         * @brief Get a reference to the texture container.
         */
        ANIMATIONS& get_animation_storage();

        /**
         * @brief Get a reference to the texture container.
         */
        FONTS& get_font_storage();

        /**
         * @brief Get a reference to the rendering mode.
         */
        GLenum& get_render_mode();

        /**
         * @brief Get flag of the debug mode.
         */
        GLboolean& get_debug_mode();

        /**
         * @brief Get a reference to the camera object.
         */
        Camera2D& get_camera();

        /**
         * @brief Get a reference to the scale flag.
         */
        int& get_scale_flag();

        /**
         * @brief Get a reference to the rotation flag.
         */
        int& get_rotation_flag();

        /**
         * @brief Get a reference to the player direction.
         */
        int& get_player_direction();

        /**
         * @brief Get a reference to the player direction.
         */
        int& get_moving_status();

        /**
         * @brief Get a reference to the player direction.
         */
        int& get_mining_status();

        /**
         * @brief Get a reference to the framebuffer object.
         */
        GLuint& get_framebuffer();

        /**
         * @brief Get a reference to the framebuffer texture object.
         */
        GLuint& get_framebuffer_texture();

        /**
         * @brief Get a reference to the editor mode flag.
         */
        int& get_editor_mode();

        /**
        * @brief Compile the shaders, link the shader objects to create an executable,
                 and ensure the program can work in the current OpenGL state.
        * @param shader_files The data which contains the shader type and its filepath.
        * @param shader The shader program that will be created, compiled, and link.
        * @return True if shader program compile and link successfully, false otherwise.
        */
        GLboolean compile_shader(std::vector<std::pair<GLenum, std::string>> shader_files, Assets_Manager::ShaderProgram& shader);

        /**
         * @brief Start the shader program
         *
         *.@param shader The shader program that is to be started
         */
         //void program_use(ShaderProgram shader);
        void program_use(GLuint program_handle);

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
         //GLuint get_shader_program_handle(ShaderProgram shader) const;
        GLuint get_shader_program_handle(Assets_Manager::ShaderProgram shader) const;
    };

} // namespace lof

#endif // LOF_GRAPHICS_MANAGER_H
