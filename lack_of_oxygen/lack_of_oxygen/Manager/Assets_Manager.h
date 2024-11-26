/**
 * @file Assets_Manager.cpp
 * @brief Implements the Assets Manager class methods.
 * @author Chua Wen Bin Kenny (99.64%), Liliana Hanawardani (0.359%), Saw Hui Shan (%)
 * @date November 8, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once

#ifndef LOF_ASSETS_MANAGER_H
#define LOF_ASSETS_MANAGER_H


// For accessing manager instances
#define ASM lof::Assets_Manager::get_instance()

// Include standard headers
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

// Include header
#include "Manager.h"

// Include other header
#include "../Utility/constant.h"  // To access constants 

// For fonts library
#include <../ft2build.h>
#include FT_FREETYPE_H 


namespace lof {


    class Assets_Manager : public Manager {
        
        friend class Graphics_Manager;      // To Graphics Manager to access the class private (Hui Shan)
        friend class Serialization_Manager; // To Serialization Manager to access the class private (Hui Shan)

    public:

        // Struct of data to create a shader (Kenny)
        struct ShaderProgram {
            GLuint program_handle = 0;
            GLboolean link_status = GL_FALSE;
        };
        
        // Struct of the vertex for model data (Kenny)
        struct TexVtxData {
            glm::vec2 pos{};
            glm::vec2 tex{};
        };

        // Struct of data to create a model (Hui Shan)
        struct ModelData {
            std::vector<TexVtxData> texVtxArr;  // For textured vertices
            std::vector<glm::vec2> posVtx;      // For position vertices
            std::vector<GLushort> vtxIdx;       // Index data
            GLenum primitiveType{};             // OpenGL primitive type (Triangle, Lines, Square etc
            bool isModelExist = false;          // Is model exist 
            std::string modelName;              // Name of the model
        };

        /**
        * @brief Virtual destrucctor for the Assets Manager (Hui Shan)
        */
        virtual ~Assets_Manager(); 

        /**
        * @brief Returns the singleton instance of Assets Manager (Hui Shan)
        */
        static Assets_Manager& get_instance();

        /**
         * @brief Initializzes the asset directory paths (Hui Shan)
         */
        void initialize_paths();

        /**
        * @brief Reads and parses a JSON file
        * @param filepath Path to JSON file
        * @param json_content store the j_son content from output string
        * @return True if success to load j_son file, else false
        */
        bool read_json_file(const std::string& filepath, std::string& json_content);

        /**
        * @brief Loads the texture
        * @param file_path 
            The path of the texture located
        * @param texture_names 
            The name of the texture 
        * @return True if success to load textures, else false
        */
        bool load_all_textures(const std::string& file_path, std::vector<std::string>& texture_names);

        /**
        * @brief Read the shader file
        * @param file_path
        *    The path of the shader to be read
        * @param shader_source 
        *    The source of the shader
        * @return True if success to read shader, else false 
        */
        bool read_shader_file(const std::string& file_path, std::string& shader_source);

        /**
        * @brief Read the shader file
        * @param file_path
        *    The path of the shader to be read
        * @param shader_source
        *    The source of the shader
        * @return True if success to read shader, else false
        */
        bool load_shader_programs(std::vector<std::pair<std::string, std::string>> shaders);

        /**
        * @brief Gets shader program by index
        * @param index
        *    The index of the shader program
        * @return The struct of the shader program
        */
        ShaderProgram* get_shader_program(size_t index);

        /**
       * @brief Unloaded the shader
       */
        void unload_shader_programs();

       /**
       * @brief Load the model data 
       * @param file_name
       *    The file name of the model
       * @return True if load successfully, else false
       */
        bool load_model_data(const std::string& file_name);

       /**
       * @brief Get the full path in order to load the file
       * @param base path 
            The base path of the program
       *@return The string of the full path
       */
        std::string get_full_path(const std::string& base_path, const std::string& name);

        /**
       * @brief Load the animation
       * @param file_name
       *    The file name of the animation
       * @return True if load successfully, else false
       */
        bool load_animations(const std::string& file_name);

        /**
       * @brief Load the font
       * @param font_name
       *    Name of the fonts
       * out_ft
       *    Reference to an FT_Library object that will manage FreeType resources
       * out_face
       *    Reference to an FT_Face object that will hold the loaded font face upon success
       * @return True if load successfully, else false
       */
        bool load_fonts(const std::string& font_name, FT_Library& out_ft, FT_Face& out_face);


       /**
       * @brief Read the font list
       * @param file_name
       *    Name of the file
       * @param out_font_names
       *    
       * @return True if load successfully, else false
       */
        bool read_font_list(const std::string& file_name, std::vector<std::string>& out_font_names);

        std::string get_executable_directory();
    private:
        // A unique_ptr to the single instance of Assets_Manager
        static std::unique_ptr<Assets_Manager> instance;
        // Used with std::call_once to initialize the singleton instance safely in a multithreaded environment.
        static std::once_flag once_flag;

        // Stores the path to the executable
        std::string executable_path;

        // An unordered_map that caches textures, mapping texture names to their respective IDs
        std::unordered_map<std::string, unsigned int> texture_cache;
        // An unordered_map that caches shader programs, mapping shader names to ShaderProgram objects
        std::unordered_map<std::string, ShaderProgram> shader_cache;

        // An unordered_map that caches models, mapping model names to ModelData objects
        std::unordered_map<std::string, ModelData> model_storage;

        // A list of ShaderProgram objects, likely to manage all loaded shader programs
        std::vector<ShaderProgram> shader_programs;
       
       /**
       * @brief See if the file is valid 
       * @param file_path
       *    The path to be valid
       * @param file
       *    If the file at file_path exists and can be opened, file is opened with that path
       * @return True if load successfully, else false
       */
        bool validate_file(const std::string& file_path, std::ifstream& file);
        
           

       /**
       * @brief Read the file content
       * @param file_path
       *    The path to be readed
       * @param content
       *    A reference to a string where the file content will be stored
       * @return True if load successfully, else false
       */
        bool read_file_content(const std::string& file_path, std::string& content);

       

        /**
       * @brief default constructor for the assets manager
       */
        Assets_Manager();
        
        /**
       * @brief The constructor that prevents copying of Assets_Manager instances.
       */
        Assets_Manager(const Assets_Manager&) = delete;

        /**
       * @brief Prevents assignment between Assets_Manager instances.
       */
        Assets_Manager& operator=(const Assets_Manager&) = delete;


        // All the necessary file path 
        const std::string AUDIO_PATH = "Audios";
        const std::string TEXTURE_PATH = "Textures";
        const std::string MODEL_PATH = "Models";
        const std::string SHADER_PATH = "Shaders";
        const std::string FONT_PATH = "Fonts";
        const std::string LEVEL_PATH = "Level_Design";
        const std::string BASE_PATH = "..\\..\\lack_of_oxygen\\Assets\\";
#ifndef _DEBUG
        const std::string BASE_PATH = "Assets\\";
#endif

    };

} // namespace lof

#endif // LOF_ASSETS_MANAGER_H



