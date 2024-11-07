/**
 * @file Serialization_Manager.h
 * @brief Defines the Serialization_Manager class for handling serialization and configuration data.
 * @author Simon Chan (100%)
 * @date September 22, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_SERIALIZATION_MANAGER_H
#define LOF_SERIALIZATION_MANAGER_H

 // Macros for accessing manager singleton instances
#define SM lof::Serialization_Manager::get_instance()

// Include base headers
#include "Manager.h"

// Include component headers
#include "../Component/Component.h"

// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

// Include standard headers
#include <string>
#include <unordered_map>

namespace lof {

    /**
     * @class Serialization_Manager
     * @brief Manages loading and providing access to configuration and serialization data.
     *
     * This class follows the Singleton pattern to ensure a single instance throughout the application's lifecycle.
     */
    class Serialization_Manager : public Manager {
    private:
        // Private constructor for singleton pattern
        Serialization_Manager();

        // Member variables
        unsigned int m_scr_width;
        unsigned int m_scr_height;
        float m_fps_display_interval;

        // RapidJSON document for general configuration
        rapidjson::Document m_document;

        // Cache for loaded prefabs
        std::unordered_map<std::string, rapidjson::Value> m_prefab_map;

        // Helper function to load the prefab file
        bool load_prefabs(const char* filepath);

        // Helper function to load configuration file
        bool load_config(const char* filepath);

        // Helper function to merge two JSON objects
        void merge_objects(const rapidjson::Value& source,
            rapidjson::Value& destination,
            rapidjson::Document::AllocatorType& allocator);

        /**
         * @brief Serializes a component into a RapidJSON Value object for saving to file.
         * @param component The component to serialize
         * @param allocator RapidJSON allocator for string/array allocation
         * @return rapidjson::Value A JSON object containing the serialized component data
         */
        rapidjson::Value serialize_transform_component(const Transform2D& component, rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value serialize_graphics_component(const Graphics_Component& component, rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value serialize_collision_component(const Collision_Component& component, rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value serialize_physics_component(const Physics_Component& component, rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value serialize_velocity_component(const Velocity_Component& component, rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value serialize_audio_component(const Audio_Component& component, rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value serialize_animation_component(const Animation_Component& component, rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value serialize_logic_component(const Logic_Component& component, rapidjson::Document::AllocatorType& allocator);

    public:

        /**
         * @brief Get the singleton instance of Serialization_Manager.
         * @return Reference to the singleton instance.
         */
        static Serialization_Manager& get_instance();

        // Delete copy constructor and assignment operator
        Serialization_Manager(const Serialization_Manager&) = delete;
        Serialization_Manager& operator=(const Serialization_Manager&) = delete;

        int start_up() override;
        void shut_down() override;

        /**
         * @brief Load a scene file and create entities and components.
         * @param filename The path to the scene file.
         * @return True on success, false on failure.
         */
        bool load_scene(const char* filename);

        // Getters for configuration data
        unsigned int get_scr_width() const;
        unsigned int get_scr_height() const;
        float get_fps_display_interval() const;

        /**
         * @brief Retrieve a prefab configuration by its name.
         * @param prefab_name The name of the prefab to retrieve.
         * @return A constant pointer to the prefab's JSON value if found; otherwise, nullptr.
         */
        const rapidjson::Value* get_prefab(const std::string& prefab_name) const;

        /**
         * @brief Saves the current game state to a JSON file in scene format.
         * @param filepath The path where the save file should be created.
         * @return True if saving was successful, false otherwise.
         */
        bool save_game_state(const char* filepath);
    };

} // namespace lof

#endif // LOF_SERIALIZATION_MANAGER_H
