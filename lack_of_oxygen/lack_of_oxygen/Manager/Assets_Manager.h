/**
 * @file Assets_Manager.cpp
 * @brief Implements the Assets Manager class methods.
 * @author Saw Hui Shan (98%), Amanda Leow Boon Suan (2%)
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
#include <unordered_set>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <map>

// Include header
#include "Manager.h"
#include "../Utility/Type.h"

// Include other header
#include "../Utility/constant.h"  // To access constants 

// For fonts library
#include <../ft2build.h>
#include FT_FREETYPE_H 


namespace lof {


    class Assets_Manager : public Manager {
        
        friend class Graphics_Manager;      
        friend class Serialization_Manager; 

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

        // Struct of data to create a model
        struct Model {
            GLenum primitive_type;
            GLuint vaoid;
            GLuint draw_cnt;
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

        
        struct ModelData {
            std::vector<TexVtxData> texVtxArr;  // For textured vertices
            std::vector<glm::vec2> posVtx;      // For position vertices
            std::vector<GLushort> vtxIdx;       // Index data
            GLenum primitiveType{};             // OpenGL primitive type (Triangle, Lines, Square etc
            bool isModelExist = false;          // Is model exist 
            std::string modelName;              // Name of the model

            //GLuint vaoid = 0;                  // Vertex Array Object ID
            //GLuint draw_cnt = 0;               // Number of elements to draw
        };

     /*   struct Model {
            GLenum primitive_type;
            GLuint vaoid;
            GLuint draw_cnt;
        };*/

        /**
        * @brief Virtual destrucctor for the Assets Manager 
        */
        virtual ~Assets_Manager(); 

        /**
        * @brief Returns the singleton instance of Assets Manager 
        */
        static Assets_Manager& get_instance();

        /**
         * @brief Initializzes the asset directory paths 
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

        /**
        * @brief Get the path of an audio resource.
        * @param audio_name
        *    The name of the audio file
        * @return
        *    A `std::string` containing the full path to the audio file.
        */
        std::string get_audio_path(const std::string& audio_name);

        /**
        * @brief Load an audio file.
        * @param audio_name
        *    The name of the audio file to load.
        * @return
        *    True if the audio file is loaded successfully; false otherwise.
        */
        bool load_audio_file(const std::string& audio_name);

        /**
        * @brief Get the directory where the executable is located.
        *
        * @return
        *    A `std::string` containing the absolute path to the executable's directory.
        */
        std::string get_executable_directory();

        ///////////////////////////////////////////////////////////////////////////////////////
        std::map<std::string, Font>& get_font_storage() {
            return font_storage;
        }

        /**
        * @brief Unload the fonts that store in the font storage.
        */
        void unload_fonts();

        /**
        * @brief Unload the models that store in the model storage.
        */
        void unload_models();

        /**
        * @brief Unload the texture that store in the texture storage.
        */
        void unload_textures();

        /**
        * @brief Unload the animation that store in the animation storage.
        */
        void unload_animations();

        /**
        * @brief Stores a font in the font storage.
        * @param font_name The name to associate with the font.
        * @param font The Font object to store.
        */
        //void store_font(const std::string& font_name, const Font& font);



        std::unordered_map<std::string, Animation>& get_animation_storage() {
            return animation_storage;
        };

        std::unordered_map<std::string, GLuint>& get_texture_storage() {
            return texture_storage;
        }

        std::unordered_map<std::string, ModelData>& get_model_storage() {
            return model_storage;
        }

        /**
        * @brief Track the entity of the texture assets with entity and component.
        * @param entity_id The ID of the entity.
        * @param component_name The name of the component using the asset.
        * @param asset_name The name of the asset being tracked.
        */
        void track_entity_asset(EntityID entity_id, const std::string& component_name, const std::string& asset_name);

        /**
        * @brief Registers an asset by name in the asset manager.
        * @param asset_name The name of the asset to register.
        */
        void register_asset(const std::string& asset_name);

        /**
        * @brief Lists all assets in the texture folder and registers them.
        */
        void all_assets_in_file();

        /**
        * @brief Shuts down the asset manager by unloading all asset types.
        */
        void shut_down() override;

        /**
        * @brief Deletes a texture by its name and delete the graphic component if the texture being delete
        * @param texture_name The name of the texture to delete.
        */
        void delete_texture(const std::string& texture_name);

        /**
        * @brief Registers a new asset from a given file path.
        * @param file_path The path to the asset file to be registered.
        */
        void register_assets_from_file(const std::string& file_path);
     
        /**
        * @brief Get the set of all registered asset names.
        * @return A reference to the unordered set containing all registered asset names.
        */
        std::unordered_set<std::string>& get_all_assets() { return all_assets; };

        /**
        * @brief Adds the assets from file extension and copy the asset to the correct destination folder in the asset directory.
        * @param filePath The path of the file to add.
        */
        void AddAsset(const std::string& filePath);

        /**
        * @brief Copy a file from the source path to the destination path.
        * @param destinationPath The target location for the copied file.
        * @param sourcePath The source file location.
        * @return True if the copy was successful, false otherwise.
        */
        bool CopyFileTo(const std::string& destinationPath, const std::string& sourcePath);

        /**
        * @brief Checks if an entity has an audio component.
        * @param entity The ID of the entity to check.
        * @return True if the entity has an audio component, false otherwise.
        */
        bool has_audio_component(const EntityID& entity);

        /**
        * @brief Checks if an entity is using a specific audio file.
        * @param entity The entity ID.
        * @param target_audio The audio file path to check.
        * @return True if the entity is using the target audio, false otherwise.
        */
        bool is_entity_using_audio(const EntityID& entity, const std::string target_audio);

        /**
        * @brief Finds and removes a specific audio file from the systema and stops sound associated with the file and removes it from storage.
        * @param target_audio The target audio file path to remove.
        * @return True if the audio was successfully removed, false otherwise.
        */
        bool find_and_remove_audio(const std::string& target_audio);

        
        /**
        * @brief Gets the entity that is using a specific audio file.
        * @param target_audio The target audio file path.
        * @return The ID of the entity using the target audio, or INVALID_ENTITY_ID if none is found.
        */
        EntityID get_entity_with_audio(const std::string& target_audio);

        /**
        * @brief Retrieves all entities that have an audio component.
        * @return A vector containing the IDs of entities with audio components.
        */
        std::vector<EntityID> get_all_entities_with_audio();

    private:

       // Tracks assets with entity and component pair
        std::unordered_map<EntityID, std::unordered_map<std::string, std::unordered_set<std::string>>> tracked_assets;
        std::unordered_set<std::string> all_assets; // Stores the name of all the register assets in the system
        //std::unordered_set<std::string> loaded_assets; // store the currrent loaded assets
        //std::unordered_set<std::string> unused_assets; // unused texture assets

        std::map<std::string, Font> font_storage; // storage for font
        std::unordered_map<std::string, GLuint> texture_storage;  // storage for texture
        std::unordered_map<std::string, Animation> animation_storage; // storage for animation 

        std::unordered_map<EntityID, std::string> track_audio_usage; // audio usage

        //std::unordered_map<EntityID, Audio_Component> asset_audio_component;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // A unique_ptr to the single instance of Assets_Manager
        static std::unique_ptr<Assets_Manager> instance;
        // Used with std::call_once to initialize the singleton instance safely in a multithreaded environment.
        static std::once_flag once_flag;

        // Stores the path to the executable
        std::string executable_path;

        std::unordered_map<std::string, std::string> audio_paths;

        // An unordered_map that caches textures, mapping texture names to their respective IDs
        //std::unordered_map<std::string, unsigned int> texture_cache;
        // An unordered_map that caches shader programs, mapping shader names to ShaderProgram objects
        std::unordered_map<std::string, ShaderProgram> shader_cache;

        // An unordered_map that caches models, mapping model names to ModelData objects
        std::unordered_map<std::string, ModelData> model_storage;

        // A list of ShaderProgram objects, likely to manage all loaded shader programs
        std::vector<ShaderProgram> shader_programs;

        //std::vector<EntityID> entities_with_audio;

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
        const std::string AUDIO_PATH = "Audio";
        const std::string TEXTURE_PATH = "Textures";
        const std::string MODEL_PATH = "Models";
        const std::string SHADER_PATH = "Shaders";
        const std::string FONT_PATH = "Fonts";
        const std::string LEVEL_PATH = "Level_Design";
#ifndef NDEBUG
        const std::string BASE_PATH = "..\\..\\lack_of_oxygen\\Assets\\";
#endif
#ifndef _DEBUG
        const std::string BASE_PATH = "Assets\\";
#endif

    };

} // namespace lof

#endif // LOF_ASSETS_MANAGER_H



