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


#define ASM lof::Assets_Manager::get_instance()

#include "Manager.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "../System/Audio_System.h"
#include "../Manager/Assets_Manager.h"
#include "Serialization_Manager.h"
#include <mutex>
#include <../ft2build.h>
#include FT_FREETYPE_H 


namespace lof {
   // class Graphics_Manager;

    class Assets_Manager : public Manager {
        
        friend class Graphics_Manager;
    public:

      
        // Struct of data to create a shader
        struct ShaderProgram {
            GLuint program_handle = 0;
            GLboolean link_status = GL_FALSE;
        };
       
        struct TexVtxData {
            glm::vec2 pos{};
            glm::vec2 tex{};
        };

        // Struct of data to create a model
        struct ModelData {
            std::vector<TexVtxData> texVtxArr;  // For textured vertices
            std::vector<glm::vec2> posVtx;      // For position-only vertices
            std::vector<GLushort> vtxIdx;       // Index data
            GLenum primitiveType{};             // OpenGL primitive type
            bool isModelExist = false;          // Model existence flag
            std::string modelName;              // Name of the model
        };


        virtual ~Assets_Manager();
        static Assets_Manager& get_instance();
        void initialize_paths();


        const std::string AUDIO_PATH = "Audios\\";
        const std::string TEXTURE_PATH = "Textures\\";
        const std::string MODEL_PATH = "Models\\";
        const std::string SHADER_PATH = "Shaders\\";
        const std::string FONT_PATH = "Fonts\\";
        const std::string BASE_PATH = "..\\..\\lack_of_oxygen\\Assets\\";

        bool read_json_file(const std::string& filepath, std::string& json_content);

        bool load_all_textures(const std::string& filepath, std::vector<std::string>& texture_names);
        void unload_texture(const std::string& name);

        bool read_shader_file(const std::string& file_path, std::string& shader_source);
        bool load_shader_programs(std::vector<std::pair<std::string, std::string>> shaders);
        ShaderProgram* get_shader_program(size_t index);
        void unload_shader_programs();


        bool load_model_data(const std::string& file_name);
        ModelData* get_model_data(const std::string& model_name);

        static const std::unordered_map<std::string, std::string> PATH_MAP;
        std::string get_full_path(const std::string& base_path, const std::string& name);

        //bool load_animations(const std::string& file_name);
        bool load_animations(const std::string& file_name);
        bool load_fonts(const std::string& font_name, FT_Library& out_ft, FT_Face& out_face);
        bool read_font_list(const std::string& file_name, std::vector<std::string>& out_font_names);


    private:
        static std::unique_ptr<Assets_Manager> instance;
        static std::once_flag once_flag;

        std::string executable_path;

        std::unordered_map<std::string, unsigned int> texture_cache;
        std::unordered_map<std::string, ShaderProgram> shader_cache;
        std::unordered_map<std::string, ModelData> model_storage;
        std::vector<ShaderProgram> shader_programs;
       

        bool validate_file(const std::string& filepath, std::ifstream& file);
        bool read_file_content(const std::string& filepath, std::string& content);

        //void store_animation(const std::string& name, const lof::Graphics_Manager::Animation& anim);
        //void store_font(const std::string& name, const lof::Graphics_Manager::Font& font);

        Assets_Manager();
        Assets_Manager(const Assets_Manager&) = delete;
        Assets_Manager& operator=(const Assets_Manager&) = delete;
    };

} // namespace lof

#endif // LOF_ASSETS_MANAGER_H



