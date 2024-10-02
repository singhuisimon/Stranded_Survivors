/**
 * @file Serialization_Manager.h
 * @brief Defines the Serialization_Manager class for handling serialization and configuration data.
 * @date September 22, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#ifndef LOF_SERIALIZATION_MANAGER_H
#define LOF_SERIALIZATION_MANAGER_H

 // Macros for accessing manager singleton instances
#define SM lof::Serialization_Manager::get_instance()

// Include base headers
#include "Manager.h"

// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

// Include standard headers
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace lof {

    /**
     * @class Serialization_Manager
     * @brief Manages loading and providing access to configuration and serialization data.
     */
    class Serialization_Manager : public Manager {
    private:
        // Private constructor for singleton pattern
        Serialization_Manager();

        // Member variables
        unsigned int m_scr_width;
        unsigned int m_scr_height;
        float m_fps_display_interval;

        // Data directory path
        std::string m_data_directory;

        // RapidJSON document for general configuration
        rapidjson::Document m_document;

        // Cache for loaded prefabs
        std::unordered_map<std::string, rapidjson::Value> m_prefab_map;

        // Helper function to load the prefab file
        bool load_prefabs(const std::string& filename);

        // Helper functions to load configuration file
        bool load_config(const std::string& filename);

        // Helper function to merge two JSON objects
        void merge_objects(const rapidjson::Value& source, 
            rapidjson::Value& destination, rapidjson::Document::AllocatorType& allocator);

    public:
        /**
         * @brief Get the singleton instance of Serialization_Manager.
         * @return Reference to the singleton instance.
         */
        static Serialization_Manager& get_instance();

        // Delete copy constructor and assignment operator
        Serialization_Manager(const Serialization_Manager&) = delete;
        Serialization_Manager& operator=(const Serialization_Manager&) = delete;

        /**
         * @brief Start up the Serialization_Manager by loading configuration files.
         * @return 0 on success, negative value on failure.
         */
        int start_up() override;

        /**
         * @brief Shut down the Serialization_Manager.
         */
        void shut_down() override;

        /**
         * @brief Load a scene file and create entities and components.
         * @param filename The path to the scene file.
         * @return True on success, false on failure.
         */
        bool load_scene(const std::string& filename);

        /**
         * @brief Get the screen width from the configuration.
         * @return Screen width.
         */
        unsigned int get_scr_width() const;

        /**
         * @brief Get the screen height from the configuration.
         * @return Screen height.
         */
        unsigned int get_scr_height() const;

        /**
         * @brief Get the FPS display interval from the configuration.
         * @return FPS display interval.
         */
        float get_fps_display_interval() const;


        /**
         * @brief Retrieve a prefab configuration by its name.
         * @param prefab_name The name of the prefab to retrieve.
         * @return A constant pointer to the prefab's JSON value if found; otherwise, nullptr.
         */
        const rapidjson::Value* get_prefab(const std::string& prefab_name) const;
    };

} // namespace lof

#endif // LOF_SERIALIZATION_MANAGER_H
