/**
 * @file Assets_Manager.cpp
 * @brief Implements the Assets_Manager.
 * @author Saw Hui Shan (%), Chua Wen Bin Kenny (%) 
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.

 */


#include "Assets_Manager.h"
#include "Graphics_Manager.h"
#include "Log_Manager.h"
#include "../System/Audio_System.h"
#include "../Utility/constant.h" 
#include <windows.h>

#include <filesystem>

namespace lof {

    // Hui Shan
    std::unique_ptr<Assets_Manager> Assets_Manager::instance;
    std::once_flag Assets_Manager::once_flag;

    // Hui Shan
    Assets_Manager::Assets_Manager() {
        set_type("Assets_Manager");
        initialize_paths();
    }

     std::string Assets_Manager::get_executable_directory() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
        }

    void Assets_Manager::initialize_paths() { // (Simon file path)
        
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        executable_path = std::string(buffer).substr(0, pos);
    }

    Assets_Manager& Assets_Manager::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Assets_Manager);
            });
        return *instance;
    }

    Assets_Manager::~Assets_Manager() {}


    bool Assets_Manager::validate_file(const std::string& filepath, std::ifstream& file) { // (Hui Shan)
        file.open(filepath, std::ios::in);
        if (!file.good()) {
            LM.write_log("Assets_Manager: Unable to open file: %s", filepath.c_str());
            return false;
        }
        else
        {
            LM.write_log("Assets_Manager: Successfully open file: %s", filepath.c_str());
        }
            
        file.seekg(0, std::ios::beg);
        return true;
    }

    bool Assets_Manager::read_file_content(const std::string& filepath, std::string& content) {
        std::ifstream file;
        if (!validate_file(filepath, file)) {
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
        file.close();

        return true;
    }

    bool Assets_Manager::read_json_file(const std::string& filepath, std::string& json_content) {
        std::ifstream ifs(filepath);
        if (!ifs.is_open()) {
            LM.write_log("Assets_Manager::read_json_file(): Failed to open file: %s", filepath.c_str());
            return false;
        }

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        json_content = buffer.str();
        return true;
    }

    std::string Assets_Manager::get_full_path(const std::string& base_path, const std::string& name) {
        std::string clean_name = name;
        while (!clean_name.empty() && (clean_name[0] == '/' || clean_name[0] == '\\')) {
            clean_name = clean_name.substr(1);
        }

        std::string full_path = executable_path + "\\" + BASE_PATH + base_path + "\\" + clean_name;
        LM.write_log("Assets_Manager: Full path: %s", full_path.c_str());
        return full_path;
    }


    bool Assets_Manager::load_all_textures(const std::string& filepath, std::vector<std::string>& texture_names) {
        std::ifstream input_file{ filepath, std::ios::in };
        if (!input_file) {
            LM.write_log("Assets_Manager: Unable to open texture list %s", filepath.c_str());
            return false;
        }

        std::string tex_name;
        while (getline(input_file, tex_name)) {

            LM.write_log("Assets_Manager: Found texture name: %s", tex_name.c_str());
            texture_names.push_back(tex_name);
        }
        input_file.close();

        LM.write_log("Assets_Manager: Loaded %d texture names", texture_names.size());
        return true;
    }


    bool Assets_Manager::read_shader_file(const std::string& file_path, std::string& shader_source) {
        // Check if file's state is good for reading
        std::ifstream input_file(file_path);
        if (input_file.good() == GL_FALSE) {
            LM.write_log("Assets_Manager: File %s has error.", file_path.c_str());
            return false;
        }

        // Read code from shader file
        std::stringstream ss;
        ss << input_file.rdbuf();
        input_file.close();

        shader_source = ss.str();
        LM.write_log("Assets_Manager: Successfully read shader file %s", file_path.c_str());
        return true;
    }

    bool Assets_Manager::load_shader_programs(std::vector<std::pair<std::string, std::string>> shaders) {
        for (auto const& file : shaders) {
            // Create the shader files vector with types
            std::vector<std::pair<GLenum, std::string>> shader_files;
            shader_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, file.first));
            shader_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, file.second));

            // Create new shader program
           ShaderProgram shader_program;

            // Use Graphics_Manager to compile the shader
            if (!GFXM.compile_shader(shader_files, shader_program)) {
                LM.write_log("Assets_Manager::load_shader_programs(): Shader program failed to compile.");
                return false;
            }

            // Insert shader program into vector
            shader_programs.emplace_back(shader_program);
            std::size_t shader_idx = shader_programs.size() - 1;

            LM.write_log("Assets_Manager::load_shader_programs(): Shader program handle is %u.",
                shader_program.program_handle);
            LM.write_log("Assets_Manager::load_shader_programs(): Shader program %zu created, compiled and added successfully.",
                shader_idx);
        }
        return true;
    }


    // Get the shader program (Hui Shan)
    Assets_Manager::ShaderProgram* Assets_Manager::get_shader_program(size_t index) {
        if (index < shader_programs.size()) {
            return &shader_programs[index];
        }
        return nullptr;
    }

    // Unload the shader as shader handle in assets manager now (Hui Shan)
    void Assets_Manager::unload_shader_programs() {
        // Delete all shader programs
        for (auto& shader : shader_programs) {
            if (shader.program_handle > 0) {
                glDeleteProgram(shader.program_handle);
                shader.program_handle = 0;
                shader.link_status = GL_FALSE;
            }
        }
        shader_programs.clear();
        shader_cache.clear();
        LM.write_log("Assets_Manager: Unloaded all shader programs");
    }

 
    bool Assets_Manager::load_model_data(const std::string& file_name) {
        std::ifstream input_file{ file_name, std::ios::in };
        if (!input_file) {
            LM.write_log("Assets_Manager: Unable to open %s", file_name.c_str());
            return false;
        }
        input_file.seekg(0, std::ios::beg);

        std::string model_name, prefix, file_line;
        ModelData currentModel;
        GLboolean is_model_exist{ GL_FALSE };

        while (getline(input_file, file_line)) {
            // Skip if model exists, just like original code
            if (is_model_exist) {
                continue;
            }

            std::istringstream file_line_ss{ file_line };
            file_line_ss >> prefix;

            if (prefix == "m") {
                file_line_ss >> model_name;
                currentModel.modelName = model_name;

                if (model_storage.find(model_name) != model_storage.end()) {
                    is_model_exist = GL_TRUE;
                    currentModel.isModelExist = true;
                }
                else {
                    is_model_exist = GL_FALSE;
                    currentModel.isModelExist = false;
                }
            }
            else if (prefix == "v" && !is_model_exist) {
                if (model_name == "square") {
                    TexVtxData data{};
                    file_line_ss >> data.pos.x >> data.pos.y >> data.tex.x >> data.tex.y;
                    currentModel.texVtxArr.push_back(data);
                }
                else {
                    glm::vec2 pos{};
                    file_line_ss >> pos.x >> pos.y;
                    currentModel.posVtx.push_back(pos);
                }
            }
            else if (prefix == "i" && !is_model_exist) {
                GLushort idx;
                while (file_line_ss >> idx) {
                    currentModel.vtxIdx.push_back(idx);
                }
            }
            else if (prefix == "t" || prefix == "f" || prefix == "s" || prefix == "l") {
                if (prefix == "t") currentModel.primitiveType = GL_TRIANGLES;
                else if (prefix == "f") currentModel.primitiveType = GL_TRIANGLE_FAN;
                else if (prefix == "s") currentModel.primitiveType = GL_TRIANGLE_STRIP;
                else if (prefix == "l") currentModel.primitiveType = GL_LINES;
            }
            else if (prefix == "e") {
                if (!is_model_exist && !currentModel.modelName.empty()) {
                    model_storage[currentModel.modelName] = currentModel;
                    LM.write_log("Assets_Manager: Loaded model data for %s", currentModel.modelName.c_str());
                }
                // Reset for next model, just like original code
                currentModel = ModelData();
                model_name = "";
                is_model_exist = false;
            }
        }

        input_file.close();
        return true;
    }

   
   // kenny code 
    bool Assets_Manager::load_animations(const std::string& file_name) {
        std::ifstream input_file{ file_name, std::ios::in };
        if (!input_file) {
            LM.write_log("Unable to open %s", file_name.c_str());
            return false;
        }

        std::string file_line, prefix;
        std::string anim_name;
        Graphics_Manager::Animation animation{};
        Graphics_Manager::Frame frame{};

        while (getline(input_file, file_line)) {
            std::istringstream file_line_ss{ file_line };
            file_line_ss >> prefix;

            if (prefix == "name") {
                file_line_ss >> anim_name;
            }
            else if (prefix == "texture") {
                file_line_ss >> animation.texture_name;
            }
            else if (prefix == "tex_width") {
                file_line_ss >> animation.tex_w;
            }
            else if (prefix == "tex_height") {
                file_line_ss >> animation.tex_h;
            }
            else if (prefix == "pos") {
                file_line_ss >> frame.uv_x;
                float temp_y;
                file_line_ss >> temp_y;
                frame.uv_y = animation.tex_h - temp_y - DEFAULT_Y_OFFSET;
            }
            else if (prefix == "size") {
                file_line_ss >> frame.size;
            }
            else if (prefix == "time_delay") {
                file_line_ss >> frame.time_delay;
            }
            else if (prefix == "EF") {
                animation.frames.emplace_back(frame);
            }
            else if (prefix == "EA") {
                animation.frame_elapsed_time = DEFAULT_FRAME_TIME_ELAPSED;
                GFXM.animation_storage[anim_name] = animation;
                animation = {};
                LM.write_log("Assets_Manager: %s animation loaded", anim_name.c_str());
            }
        }

        input_file.close();
        return true;
    }

    // kenny 
    bool Assets_Manager::load_fonts(const std::string& font_name, FT_Library& out_ft, FT_Face& out_face) {
        if (FT_Init_FreeType(&out_ft)) {
            LM.write_log("Assets_Manager: Could not initialize FreeType Library");
            return false;
        }

        //std::string font_filepath = get_executable_directory() + "\\..\\..\\lack_of_oxygen\\lack_of_oxygen\\Assets\\Fonts\\Fonts.txt";

        std::string font_filepath = "../../lack_of_oxygen/Assets/Fonts/" + font_name + ".ttf";
        if (!std::filesystem::exists(font_filepath))
        {
            font_filepath = "../lack_of_oxygen/Assets/Fonts/" + font_name + ".ttf";
        }
        std::ifstream ifs{ font_filepath, std::ios::binary };
        if (!ifs) {
            LM.write_log("Assets_Manager: Font file does not exist: %s", font_filepath.c_str());
            return false;
        }
        ifs.close();

        if (FT_New_Face(out_ft, font_filepath.c_str(), 0, &out_face)) {
            LM.write_log("Assets_Manager:: Failed to load font %s", font_name.c_str());
            return false;
        }

        return true;
    }

    // kenny 
    bool Assets_Manager::read_font_list(const std::string& file_name, std::vector<std::string>& out_font_names) {
        std::ifstream input_file{ file_name, std::ios::in };
        if (!input_file) {
            LM.write_log("Assets_Manager:: Unable to open %s", file_name.c_str());
            return false;
        }

        std::string font_name;
        while (getline(input_file, font_name)) {
            out_font_names.push_back(font_name);
        }
        input_file.close();
        return true;
    }

    std::string Assets_Manager::get_audio_path(const std::string& audio_name) {
        return get_full_path("Audio", audio_name + ".wav");
    }
#if 1
    bool Assets_Manager::load_audio_file(const std::string& audio_name) {
        std::string full_path = get_audio_path(audio_name);

        // Check if file exists and is readable
        std::ifstream file(full_path, std::ios::binary);
        if (!file.good()) {
            LM.write_log("Assets_Manager: Failed to load audio file: %s", full_path.c_str());
            return false;
        }
        file.close();

        LM.write_log("Assets_Manager: Successfully loaded audio file: %s", full_path.c_str());
        return true;
    }
#endif




    
} // namespace lof




