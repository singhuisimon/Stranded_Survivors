/**
 * @file Assets_Manager.cpp
 * @brief Implements the Assets_Manager.
 * @author Saw Hui Shan (90%), Chua Wen Bin Kenny (10%)
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

 
    std::unique_ptr<Assets_Manager> Assets_Manager::instance;
    std::once_flag Assets_Manager::once_flag;
    
    Assets_Manager::Assets_Manager() {
        set_type("Assets_Manager");
        initialize_paths();
        all_assets_in_file();
    }

     std::string Assets_Manager::get_executable_directory() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
        }

    void Assets_Manager::initialize_paths() { 
        
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


    bool Assets_Manager::validate_file(const std::string& filepath, std::ifstream& file) { 
        file.open(filepath, std::ios::in);
        if (!file.good()) {
           // LM.write_log("Assets_Manager: Unable to open file: %s", filepath.c_str());
            return false;
        }
        else
        {
           // LM.write_log("Assets_Manager: Successfully open file: %s", filepath.c_str());
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
            //LM.write_log("Assets_Manager::read_json_file(): Failed to open file: %s", filepath.c_str());
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
        //LM.write_log("Assets_Manager: Full path: %s", full_path.c_str());
        return full_path;
    }

    bool Assets_Manager::read_shader_file(const std::string& file_path, std::string& shader_source) {
        // Check if file's state is good for reading
        std::ifstream input_file(file_path);
        if (input_file.good() == GL_FALSE) {
           // LM.write_log("Assets_Manager: File %s has error.", file_path.c_str());
            return false;
        }

        // Read code from shader file
        std::stringstream ss;
        ss << input_file.rdbuf();
        input_file.close();

        shader_source = ss.str();
        //LM.write_log("Assets_Manager: Successfully read shader file %s", file_path.c_str());
        return true;
    }

    // Create, compile and check if shader programs are created successfully
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
                //LM.write_log("Assets_Manager::load_shader_programs(): Shader program failed to compile.");
                return false;
            }

            // Insert shader program into vector
            shader_programs.emplace_back(shader_program);
            std::size_t shader_idx = shader_programs.size() - 1;

            //LM.write_log("Assets_Manager::load_shader_programs(): Shader program handle is %u.",
               // shader_program.program_handle);
            //LM.write_log("Assets_Manager::load_shader_programs(): Shader program %zu created, compiled and added successfully.",
              //  shader_idx);
        }
        return true;
    }


    // Get the shader program 
    Assets_Manager::ShaderProgram* Assets_Manager::get_shader_program(size_t index) {
        if (index < shader_programs.size()) {
            return &shader_programs[index];
        }
        return nullptr;
    }

    // Unload the shader as shader handle in assets manager
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

    // Read from file and store model data
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
                    //LM.write_log("Assets_Manager: Loaded model data for %s", currentModel.modelName.c_str());
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

    // Read from file and store animation data 
    bool Assets_Manager::load_animations(const std::string& file_name) {
        std::ifstream input_file{ file_name, std::ios::in };
        if (!input_file) {
            LM.write_log("Unable to open %s", file_name.c_str());
            return false;
        }

        std::string file_line, prefix;
        std::string anim_name;
        Assets_Manager::Animation animation{};
        Assets_Manager::Frame frame{};

        while (getline(input_file, file_line)) {
            std::istringstream file_line_ss{ file_line };
            file_line_ss >> prefix;

            if (prefix == "name") {
                file_line_ss >> anim_name;
            }
            else if (prefix == "texture") {
                file_line_ss >> animation.texture_name;
                //LM.write_log("Assets_Manager: animation texture name: %s", animation.texture_name.c_str());
            }
            else if (prefix == "frame_no") {
                file_line_ss >> frame.frame_number;
            }
            else if (prefix == "time_delay") {
                file_line_ss >> frame.time_delay;
            }
            else if (prefix == "EF") {
                animation.frames.emplace_back(frame);
            }
            else if (prefix == "EA") {
           
                animation.frame_elapsed_time = DEFAULT_FRAME_TIME_ELAPSED;
                ASM.animation_storage[anim_name] = animation;
                animation = {};
                //LM.write_log("Assets_Manager: %s animation loaded", anim_name.c_str());
            }
        }

        input_file.close();
        return true;
    }

    // Load and store font data from name 
    bool Assets_Manager::load_fonts(const std::string& font_name, FT_Library& out_ft, FT_Face& out_face) {
        if (FT_Init_FreeType(&out_ft)) {
            //LM.write_log("Assets_Manager: Could not initialize FreeType Library");
            return false;
        }

        
#ifndef NDEBUG
        std::string font_filepath = "../../lack_of_oxygen/Assets/Fonts/" + font_name + ".ttf";
#endif
#ifndef _DEBUG
        std::string font_filepath = BASE_PATH + FONT_PATH + "\\" + font_name + ".ttf";
#endif

        if (!std::filesystem::exists(font_filepath))
        {
            font_filepath = "../lack_of_oxygen/Assets/Fonts/" + font_name + ".ttf";
        }
        std::ifstream ifs{ font_filepath, std::ios::binary };
        if (!ifs) {
            //LM.write_log("Assets_Manager: Font file does not exist: %s", font_filepath.c_str());
            return false;
        }
        ifs.close();

        if (FT_New_Face(out_ft, font_filepath.c_str(), 0, &out_face)) {
            //LM.write_log("Assets_Manager:: Failed to load font %s", font_name.c_str());
            return false;
        }

        return true;
    }

    void Assets_Manager::unload_fonts()
    {
        font_storage.clear();
    }

    void Assets_Manager::store_font(const std::string& font_name, const Font& font) {
        font_storage[font_name] = font;
    }

    // Read and store names of fonts 
    bool Assets_Manager::read_font_list(const std::string& file_name, std::vector<std::string>& out_font_names) {
        std::ifstream input_file{ file_name, std::ios::in };
        if (!input_file) {
            //LM.write_log("Assets_Manager:: Unable to open %s", file_name.c_str());
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


    bool Assets_Manager::load_audio_file(const std::string& audio_name) {
        std::string full_path = get_audio_path(audio_name);

        // Check if file exists and is readable
        std::ifstream file(full_path, std::ios::binary);
        if (!file.good()) {
            //LM.write_log("Assets_Manager: Failed to load audio file: %s", full_path.c_str());
            return false;
        }
        file.close();

        //LM.write_log("Assets_Manager: Successfully loaded audio file: %s", full_path.c_str());
        return true;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void Assets_Manager::shut_down() {
        // Unload assets
        unload_textures();
        unload_shader_programs();
        unload_models();
        unload_animations();
        unload_fonts();

        LM.write_log("Assets Manager and other resources have been shut down.");
    }


    void Assets_Manager::unload_models()
    {
        model_storage.clear();
    }


    void Assets_Manager::unload_textures() {
        /*for (auto& pair : texture_storage) {
            glDeleteTextures(1, &pair.second);
        }*/
        texture_storage.clear();
        std::cout << "Assets_Manager: All textures have been unloaded." << std::endl;
    }

    void Assets_Manager::unload_animations()
    {
        animation_storage.clear();
    }

    void Assets_Manager::track_entity_asset(EntityID entity_id, const std::string& component_name, const std::string& asset_name) {
        tracked_assets[entity_id][component_name].emplace(asset_name);

        LM.write_log("Tracking asset: Entity ID: %u, Component: %s, Asset: %s",
                 entity_id, component_name.c_str(), asset_name.c_str());

    }

    void Assets_Manager::register_asset(const std::string& asset_name) {
        all_assets.emplace(asset_name);
        //LM.write_log("Registered asset: %s", asset_name.c_str());
    }

    // to register new assets
    void Assets_Manager::register_assets_from_file(const std::string& file_path)
    {
        std::string asset_name = std::filesystem::path(file_path).stem().string();
        register_asset(asset_name);
        LM.write_log("Registered new asset: %s", asset_name.c_str());
    }

    // to get the items in the texture folder
    void Assets_Manager::all_assets_in_file()
    {
        std::string tex_directory = get_full_path(TEXTURE_PATH, "");

        if (!std::filesystem::exists(tex_directory))
        {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(tex_directory))
        {
            if (entry.is_regular_file())
            {
                std::string texture_name = entry.path().stem().string(); //get file name 
                register_asset(texture_name);
                LM.write_log("Registered texture asset: %s", texture_name.c_str());
            }


        }
    }

    void Assets_Manager::delete_texture(const std::string& texture_name) {
        // Check if texture exists in storage
        auto it = texture_storage.find(texture_name);
        if (it != texture_storage.end()) {
            // Delete the OpenGL texture
            GLuint tex_id = it->second;
            glDeleteTextures(1, &tex_id);
            texture_storage.erase(it);

            // Log the deletion
            LM.write_log("Assets_Manager: Deleted texture asset: %s", texture_name.c_str());
        }
        else {
            LM.write_log("Assets_Manager: Texture %s not found in storage", texture_name.c_str());
            return;
        }

        // remove Graphics Components
        for (auto& entity : ECSM.get_entities()) {
            if (entity && ECSM.has_component<Graphics_Component>(entity->get_id())) {
                Graphics_Component& graphics = ECSM.get_component<Graphics_Component>(entity->get_id());

                if (graphics.texture_name == texture_name) {
                    // If the component is using the deleted texture, remove it
                    ECSM.remove_component<Graphics_Component>(entity->get_id());
                    LM.write_log("Assets_Manager: Removed Graphics Component from Entity %u due to texture deletion: %s",
                        entity->get_id(), texture_name.c_str());
                }
            }
        }
    }

    //void Assets_Manager::delete_font(const std::string& text_name)
    //{
    //    auto it = font_storage.find(text_name);
    //    if (it != font_storage.end())
    //    {
    //        // remove the font
    //        font_storage.erase(it);
    //    }
    //    else {
    //        return;
    //    }

    //    for (auto& entity : ECSM.get_entities())
    //    {
    //        if (entity && ECSM.has_component<Text_Component>(entity->get_id()))
    //        {
    //            Text_Component& font_component = ECSM.get_component<Text_Component>(entity->get_id());
    //            if (font_component.font_name == text_name)
    //            {
    //                ECSM.remove_component<Text_Component>(entity->get_id());
    //            }
    //        }
    //    }

    //}

#if 1
    void Assets_Manager::delete_font(const std::string& text_name) {
        // Log the font deletion process
        LM.write_log("Assets_Manager: Deleting font: %s", text_name.c_str());

        // Manually iterate through the font_storage map
        for (auto font = font_storage.begin(); font != font_storage.end(); ) {
            if (font->first == text_name) {
                // If the font name matches, erase it from the map
                LM.write_log("Assets_Manager: Erasing font: %s", text_name.c_str());
                font = font_storage.erase(font);  // Safely erase and get the next iterator
            }
            else {
                ++font;  // Proceed to the next font if no deletion occurred
            }
        }

      
        for (auto& entity : ECSM.get_entities()) {
            if (entity && ECSM.has_component<Text_Component>(entity->get_id())) {
                Text_Component& font_component = ECSM.get_component<Text_Component>(entity->get_id());
                if (font_component.font_name == text_name) {
                    // remove the entire entity
                   
                    ECSM.remove_component<Text_Component>(entity->get_id());
                    LM.write_log("Assets_Manager: Removed Entity %u due to font deletion: %s",
                        entity->get_id(), text_name.c_str());
                }
            }
        }
    }
#endif 
#if 0
    void Assets_Manager::delete_font(const std::string& text_name) {
        LM.write_log("Assets_Manager: Deleting font: %s", text_name.c_str());

        // First, gather entities to remove components from
        std::vector<EntityID*> entities_to_remove;

        // Manually iterate through the font_storage map
        for (auto font = font_storage.begin(); font != font_storage.end();) {
            if (font->first == text_name) {
                LM.write_log("Assets_Manager: Erasing font: %s", text_name.c_str());
                font = font_storage.erase(font);  // Safely erase and get the next iterator
            }
            else {
                ++font;
            }
        }

        // Now remove the font component from entities
        for (auto& entity : ECSM.get_entities()) {
            if (entity && ECSM.has_component<Text_Component>(entity->get_id())) {
                Text_Component& font_component = ECSM.get_component<Text_Component>(entity->get_id());
                if (font_component.font_name == text_name) {
                    // Push the address of entity (if it's an object, not a pointer)
                    entities_to_remove.push_back(&entity);  // Address of entity
                    LM.write_log("Assets_Manager: Found Entity %u with font: %s", entity->get_id(), text_name.c_str());
                }
            }
        }

        // Now perform the actual removal of components
        for (auto* entity : entities_to_remove) {  // Dereference the pointer when accessing
            ECSM.remove_component<Text_Component>(entity->get_id());
            LM.write_log("Assets_Manager: Removed Entity %u due to font deletion: %s", entity->get_id(), text_name.c_str());
        }
    }
#endif

    //void Assets_Manager::delete_font(const std::string& text_name) {
    //    // Log the font deletion process
    //    LM.write_log("Assets_Manager: Deleting font: %s", text_name.c_str());

    //    // Manually iterate through the font_storage map
    //    for (auto font = font_storage.begin(); font != font_storage.end(); ) {
    //        if (font->first == text_name) {
    //            // If the font name matches, erase it from the map
    //            LM.write_log("Assets_Manager: Erasing font: %s", text_name.c_str());
    //            font = font_storage.erase(font);  // Safely erase and get the next iterator
    //        }
    //        else {
    //            ++font;  // Proceed to the next font if no deletion occurred
    //        }
    //    }

    //    // Create a list to store entities to be removed
    //    std::vector<EntityID> entities_to_remove;

    //    // Now, check each entity and gather those to be removed
    //    for (auto& entity : ECSM.get_entities()) {
    //        if (entity && ECSM.has_component<Text_Component>(entity->get_id())) {
    //            Text_Component& font_component = ECSM.get_component<Text_Component>(entity->get_id());
    //            if (font_component.font_name == text_name) {
    //                // Add the entity to the removal list
    //                entities_to_remove.push_back(entity->get_id());
    //                LM.write_log("Assets_Manager: Found Entity %u with font: %s", entity->get_id(), text_name.c_str());
    //            }
    //        }
    //    }

    //    // Now, attempt to remove the entire entity after removing the component
    //    for (auto& entity_id : entities_to_remove) {
    //        // Ensure we first remove the component before destroying the entity
    //        ECSM.remove_component<Text_Component>(entity_id);
    //        LM.write_log("Assets_Manager: Removed Text_Component from Entity %u due to font deletion: %s", entity_id, text_name.c_str());

    //        // Now, safely destroy the entity
    //        ECSM.destroy_entity(entity_id);
    //        LM.write_log("Assets_Manager: Destroyed Entity %u due to font deletion: %s", entity_id, text_name.c_str());
    //    }
    //}






    void Assets_Manager::AddAsset(const std::string& filePath) {
        // Extract the file extension
        std::string extension = filePath.substr(filePath.find_last_of('.') + 1);
        std::string fileName = filePath.substr(filePath.find_last_of("/\\") + 1);

        // Determine the destination subfolder within Assets
        std::string destinationSubfolder;

        if (extension == "png") {
            destinationSubfolder = TEXTURE_PATH;
        }
        else if (extension == "obj") {
            destinationSubfolder = MODEL_PATH;
        }
        else if (extension == "wav") {
            destinationSubfolder = AUDIO_PATH;
        }
        else if (extension == "vert" || extension == "frag") {  
            destinationSubfolder = SHADER_PATH;
        }
        else if (extension == "ttf") {
            destinationSubfolder = FONT_PATH;
        }
        else if (extension == "csv")
        {
            destinationSubfolder = LEVEL_PATH;
        }
        else {
            //std::cout << "Unsupported asset type: " << filePath << "\n";
            LM.write_log("Assets_Manager: Unsupported asset type %s being added.", filePath);
            return;
        }

        // Build the target file path within the Assets directory
        std::string targetPath = get_full_path(destinationSubfolder, fileName);;

        // Ensure the directory exists and copy the file
        if (CopyFileTo(targetPath, filePath)) {
            //std::cout << "File successfully added to: " << targetPath << "\n";
            LM.write_log("Assets_Manager: File %s successfully added to: %s.", fileName, targetPath);
        }
        else {
            //std::cout << "Failed to add file: " << filePath << "\n";
            LM.write_log("Assets_Manager: Failed to add file %s", filePath);
        }
    }

    bool Assets_Manager::CopyFileTo(const std::string& destinationPath, const std::string& sourcePath) {
        try {
            
            std::filesystem::create_directories(std::filesystem::path(destinationPath).parent_path());

            // Open the source file and destination file in binary mode
            std::ifstream src(sourcePath, std::ios::binary);
            std::ofstream dst(destinationPath, std::ios::binary);

            if (!src.is_open() || !dst.is_open()) {
                //std::cerr << "Error opening source or destination file.\n";
                LM.write_log("Error opening source or destination file.");
                return false;
            }

            dst << src.rdbuf();  // Copy the file content
            return true;
        }
        catch (const std::exception& e) {
            //std::cerr << "Error copying file: " << e.what() << std::endl;
            LM.write_log("Error copying file");
            return false;
        }
    }

    // to check if the entity have the audio component, true (have), false (dont have)
    bool Assets_Manager::has_audio_component(const EntityID& entity)
    {
        return ECSM.has_component<Audio_Component>(entity);
    }

    bool Assets_Manager::is_entity_using_audio(const EntityID& entity, const std::string target_audio)
    {
        // if contain audio component
        if (ECSM.has_component<Audio_Component>(entity))
        {
            auto audio_component = ECSM.get_component<Audio_Component>(entity);

            const auto& sounds = audio_component.get_sounds();
            for (const auto& sound : sounds)
            {
                if (sound.filepath == target_audio)
                {
                    std::cout << "yes! entity " << entity << "has audio component " << "\n";
                    track_audio_usage[entity] = target_audio;
                    return true;
                }
            }
        }
        return false;
    }


    EntityID Assets_Manager::get_entity_with_audio(const std::string& target_audio) {
        for (const auto& entity_ptr : ECSM.get_entities()) {
            EntityID entity = entity_ptr->get_id();

            if (ECSM.has_component<Audio_Component>(entity)) {
                Audio_Component& audio_component = ECSM.get_component<Audio_Component>(entity);

                const auto& sounds = audio_component.get_sounds();
                for (const auto& sound : sounds) {
                    if (sound.filepath == target_audio) {
                        std::cout << "Found entity " << entity << " with audio: " << target_audio << "\n";
                        track_audio_usage[entity] = target_audio;  // Store result if needed
                        return entity;
                    }
                }
            }
        }

        return INVALID_ENTITY_ID;
    }

    std::vector<EntityID> Assets_Manager::get_all_entities_with_audio() {
        std::vector<EntityID> entities_with_audio;

        for (const auto& entity_ptr : ECSM.get_entities()) {
            EntityID entity = entity_ptr->get_id();

            if (ECSM.has_component<Audio_Component>(entity)) {
                // Add this entity to the list if it has an Audio_Component
                entities_with_audio.push_back(entity);
            }
        }

        return entities_with_audio;
    }


#if 1
    bool Assets_Manager::find_and_remove_audio(const std::string& target_audio) {
        EntityID entity_with_audio = get_entity_with_audio(target_audio);

        if (entity_with_audio == INVALID_ENTITY_ID) {
            std::cout << "No entity found with the audio component for: " << target_audio << std::endl;
            return false;
        }

        // Remove the audio sound and update the channel/sound map
        Audio_Component& audio = ECSM.get_component<Audio_Component>(entity_with_audio);
        auto& sounds = audio.get_sounds();
        std::string sound_key;

        // Find the sound key
        for (const auto& sound : sounds) {
            if (sound.filepath == target_audio) {
                sound_key = sound.key;
                break;
            }
        }

        if (!sound_key.empty()) {
            std::string key_for_channel_map = target_audio + std::to_string(entity_with_audio) + sound_key;

            // Stop the sound and clean up the channel map
            for (auto& system : ECSM.get_systems()) {
                if (system->get_type() == "Audio_System") {
                    auto* audio_system = static_cast<Audio_System*>(system.get());
                    if (audio_system) {
                        audio_system->stop_sound(key_for_channel_map);
                        std::cout << "Deleted " << key_for_channel_map << " from channel map\n";
                    }

                    // Remove from the sound map if it exists
                    auto& sound_map = ADM.get_sound_map();
                    if (sound_map.erase(target_audio) > 0) {
                        std::cout << "Successfully removed from sound_map.\n";
                    }
                    else {
                        std::cout << "Sound was not found in sound_map.\n";
                    }
                }
            }

            // Small delay to ensure the system releases file
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return true;
        }

        return false;
    }

#endif

#if 0
    bool Assets_Manager::find_and_remove_audio(const std::string& target_audio) {
        for (const auto& entity_ptr : ECSM.get_entities()) {
            EntityID entity = entity_ptr->get_id();

            std::cout << "Assets manager: check audio entity " << entity << "\n";

            if (is_entity_using_audio(entity, target_audio)) {
                // Remove the audio sound and update the channel/sound map
                Audio_Component& audio = ECSM.get_component<Audio_Component>(entity);
                auto& sounds = audio.get_sounds();
                std::string sound_key;

                // Find the sound key
                for (const auto& sound : sounds) {
                    if (sound.filepath == target_audio) {
                        sound_key = sound.key;
                        break;
                    }
                }

                if (!sound_key.empty()) {
                    std::string key_for_channel_map = target_audio + std::to_string(entity) + sound_key;

                    // Stop the sound and clean up the channel map
                    for (auto& system : ECSM.get_systems()) {
                        if (system->get_type() == "Audio_System") {
                            auto* audio_system = static_cast<Audio_System*>(system.get());
                            if (audio_system) {
                                audio_system->stop_sound(key_for_channel_map);
                                std::cout << "Deleted " << key_for_channel_map << " from channel map\n";
                            }

                            // Remove from the sound map if it exists
                            auto& sound_map = ADM.get_sound_map();
                            if (sound_map.find(target_audio) != sound_map.end()) {
                                sound_map.erase(target_audio);
                                std::cout << "Successfully removed from sound_map.\n";
                            }
                            else {
                                std::cout << "Sound was not found in sound_map.\n";
                            }
                        }
                    }

                    // Small delay to ensure the system releases file
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    return true;
                }
            }
        }
        std::cout << "No entity with the audio component found for: " << target_audio << std::endl;
        return false;
    }
#endif

   


 


} // namespace lof




