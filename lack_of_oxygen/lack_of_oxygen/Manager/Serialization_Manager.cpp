/**
 * @file Serialization_Manager.cpp
 * @brief Implements the Serialization_Manager class methods.
 * @author Simon Chan
 * @date September 22, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Serialization_Manager.h"

// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

// Include Log_Manager
#include "Log_Manager.h"

// Include ECS_Manager for entity creation
#include "ECS_Manager.h"

// Include all component headers
#include "../Component/Component.h"

// Include Utility headers
#include "../Utility/Matrix3x3.h"
#include "../Utility/Component_Parser.h" // Adding components from JSON
#include "../Utility/Constant.h"

// Include standard headers
#include <fstream>
#include <sstream>
#include <cassert>

namespace lof {

    /**
     * @brief Private constructor for the singleton pattern.
     * Initializes member variables with default values.
     */
    Serialization_Manager::Serialization_Manager()
        : m_scr_width(DEFAULT_SCREEN_WIDTH),
        m_scr_height(DEFAULT_SCREEN_HEIGHT),
        m_fps_display_interval(DEFAULT_FPS_DISPLAY_INTERVAL) {
        set_type("Serialization_Manager");

        LM.write_log("Serialization_Manager::Serialization_Manager(): Initialized with default configurations.");
    }

    /**
     * @brief Retrieves the singleton instance of Serialization_Manager.
     * @return Reference to the singleton instance.
     */
    Serialization_Manager& Serialization_Manager::get_instance() {
        static Serialization_Manager instance;
        return instance;
    }

    /**
     * @brief Starts up the Serialization_Manager by loading configuration, prefabs, and scene files.
     * @return 0 on success, negative value on failure.
     */
    int Serialization_Manager::start_up() {
        m_is_started = true;

        // Load general configuration
        if (!load_config(CONFIG_PATH)) { // Use CONFIG_PATH from Constant.h
            LM.write_log("Serialization_Manager::start_up(): Failed to load game configuration file: %s", CONFIG_PATH);
            return -1; // Error loading configuration file
        }

        // Load prefabs
        if (!load_prefabs(PREFABS_PATH)) { // Use PREFABS_PATH from Constant.h
            LM.write_log("Serialization_Manager::start_up(): Failed to load prefab file: %s", PREFABS_PATH);
            return -2; // Error loading prefab file
        }

        // Load scene file
        if (!load_scene(SCENE_PATH)) { // Use SCENE_PATH from Constant.h
            LM.write_log("Serialization_Manager::start_up(): Failed to load scene file: %s", SCENE_PATH);
            return -3; // Error loading scene file
        }

        // All files loaded successfully
        LM.write_log("Serialization_Manager::start_up(): Serialization_Manager started successfully.");
        return 0;
    }

    /**
     * @brief Shuts down the Serialization_Manager by clearing loaded data and caches.
     */
    void Serialization_Manager::shut_down() {
        LM.write_log("Serialization_Manager::shut_down(): Shutting down Serialization_Manager.");
        m_document.SetNull();           // Clear the document
        m_prefab_map.clear();           // Clear the prefab cache
        m_is_started = false;
    }

    /**
     * @brief Merges two JSON objects. Source object's members are merged into the destination object.
     * If a member exists in both objects and both are objects, the merge is performed recursively.
     * Otherwise, the destination's member is overwritten by the source's member.
     *
     * @param source The source JSON object to merge from.
     * @param destination The destination JSON object to merge into.
     * @param allocator The RapidJSON allocator to use for memory management.
     */
    void Serialization_Manager::merge_objects(const rapidjson::Value& source,
        rapidjson::Value& destination,
        rapidjson::Document::AllocatorType& allocator) {
        LM.write_log("Serialization_Manager::merge_objects(): Merging objects from source to destination.");

        assert(source.IsObject());
        assert(destination.IsObject());

        for (auto itr = source.MemberBegin(); itr != source.MemberEnd(); ++itr) {
            const char* key = itr->name.GetString();

            if (destination.HasMember(key)) {
                // If both values are objects, merge them recursively
                if (itr->value.IsObject() && destination[key].IsObject()) {
                    merge_objects(itr->value, destination[key], allocator);
                }
                else {
                    // Overwrite the value in the destination
                    destination[key].CopyFrom(itr->value, allocator);
                }
            }
            else {
                // Add the new key-value pair to the destination
                rapidjson::Value name(key, allocator);
                rapidjson::Value value;
                value.CopyFrom(itr->value, allocator);
                destination.AddMember(name, value, allocator);
            }
        }
    }

    /**
     * @brief Loads the configuration file and initializes screen settings and FPS display interval.
     *
     * @param filepath The complete path to the configuration file.
     * @return True if loading and parsing are successful, false otherwise.
     */
    bool Serialization_Manager::load_config(const char* filepath) {
        LM.write_log("Serialization_Manager::load_config(): Attempting to load configuration file from: %s", filepath);

        // Read the JSON file into a string
        std::ifstream ifs(filepath);
        if (!ifs.is_open()) {
            LM.write_log("Serialization_Manager::load_config(): Failed to open configuration file: %s", filepath);
            return false;
        }

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string json_content = buffer.str();

        // Parse the JSON content
        m_document.Parse(json_content.c_str());

        if (m_document.HasParseError()) {
            size_t offset = m_document.GetErrorOffset();
            rapidjson::ParseErrorCode error_code = m_document.GetParseError();
            LM.write_log("Serialization_Manager::load_config(): JSON parse error at offset %zu: %s", offset, rapidjson::GetParseError_En(error_code));
            return false;
        }

        if (!m_document.IsObject()) {
            LM.write_log("Serialization_Manager::load_config(): Invalid JSON format: Root element is not an object.");
            return false;
        }

        // Retrieve and store the configuration values
        if (m_document.HasMember("SCR_WIDTH") && m_document["SCR_WIDTH"].IsUint()) {
            m_scr_width = m_document["SCR_WIDTH"].GetUint();
            LM.write_log("Serialization_Manager::load_config(): Loaded SCR_WIDTH: %u", m_scr_width);
        }
        else {
            LM.write_log("Serialization_Manager::load_config(): SCR_WIDTH is missing or not an unsigned integer. Using default value: %u", m_scr_width);
        }

        if (m_document.HasMember("SCR_HEIGHT") && m_document["SCR_HEIGHT"].IsUint()) {
            m_scr_height = m_document["SCR_HEIGHT"].GetUint();
            LM.write_log("Serialization_Manager::load_config(): Loaded SCR_HEIGHT: %u", m_scr_height);
        }
        else {
            LM.write_log("Serialization_Manager::load_config(): SCR_HEIGHT is missing or not an unsigned integer. Using default value: %u", m_scr_height);
        }

        if (m_document.HasMember("FPS_DISPLAY_INTERVAL") && m_document["FPS_DISPLAY_INTERVAL"].IsNumber()) {
            m_fps_display_interval = m_document["FPS_DISPLAY_INTERVAL"].GetFloat();
            LM.write_log("Serialization_Manager::load_config(): Loaded FPS_DISPLAY_INTERVAL: %.2f", m_fps_display_interval);
        }
        else {
            LM.write_log("Serialization_Manager::load_config(): FPS_DISPLAY_INTERVAL is missing or not a number. Using default value: %.2f", m_fps_display_interval);
        }

        LM.write_log("Serialization_Manager::load_config(): Configuration loaded successfully.");
        return true;
    }

    /**
     * @brief Loads the prefab file and caches the prefab definitions for quick access.
     *
     * @param filepath The complete path to the prefab file.
     * @return True if loading and parsing are successful, false otherwise.
     */
    bool Serialization_Manager::load_prefabs(const char* filepath) {
        LM.write_log("Serialization_Manager::load_prefabs(): Attempting to load prefabs from: %s", filepath);

        // Read and parse the prefab file
        std::ifstream ifs(filepath);
        if (!ifs.is_open()) {
            LM.write_log("Serialization_Manager::load_prefabs(): Failed to open prefab file: %s", filepath);
            return false;
        }

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string json_content = buffer.str();

        rapidjson::Document prefab_document;
        prefab_document.Parse(json_content.c_str());

        if (prefab_document.HasParseError()) {
            size_t offset = prefab_document.GetErrorOffset();
            rapidjson::ParseErrorCode error_code = prefab_document.GetParseError();
            LM.write_log("Serialization_Manager::load_prefabs(): JSON parse error in prefab file at offset %zu: %s", offset, rapidjson::GetParseError_En(error_code));
            return false;
        }

        if (!prefab_document.IsObject() || !prefab_document.HasMember("prefabs") || !prefab_document["prefabs"].IsObject()) {
            LM.write_log("Serialization_Manager::load_prefabs(): Invalid prefab file format: 'prefabs' object is missing or invalid.");
            return false;
        }

        // Load prefabs into the cache
        const rapidjson::Value& prefabs = prefab_document["prefabs"];
        for (auto it = prefabs.MemberBegin(); it != prefabs.MemberEnd(); ++it) {
            std::string prefab_name = it->name.GetString();

            // Deep copy the prefab definition into the map
            rapidjson::Value prefab_value(rapidjson::kObjectType);
            prefab_value.CopyFrom(it->value, m_document.GetAllocator());

            m_prefab_map.emplace(prefab_name, std::move(prefab_value));
            LM.write_log("Serialization_Manager::load_prefabs(): Loaded prefab '%s' into cache.", prefab_name.c_str());
        }

        LM.write_log("Serialization_Manager::load_prefabs(): Prefabs loaded successfully.");
        return true;
    }

    /**
     * @brief Loads the scene file, creates entities, and assigns components based on the scene and prefabs.
     *
     * @param filename The complete path to the scene file.
     * @return True if loading and parsing are successful, false otherwise.
     */
    bool Serialization_Manager::load_scene(const char* filename) {
        LM.write_log("Serialization_Manager::load_scene(): Attempting to load scene file from: %s", filename);

        // Read and parse the scene file
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            LM.write_log("Serialization_Manager::load_scene(): Failed to open scene file: %s", filename);
            return false;
        }

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string json_content = buffer.str();

        rapidjson::Document scene_document;
        scene_document.Parse(json_content.c_str());

        if (scene_document.HasParseError()) {
            size_t offset = scene_document.GetErrorOffset();
            rapidjson::ParseErrorCode error_code = scene_document.GetParseError();
            LM.write_log("Serialization_Manager::load_scene(): JSON parse error at offset %zu: %s", offset, rapidjson::GetParseError_En(error_code));
            return false;
        }

        if (!scene_document.IsObject() || !scene_document.HasMember("objects") || !scene_document["objects"].IsArray()) {
            LM.write_log("Serialization_Manager::load_scene(): Invalid scene file format: 'objects' array is missing or invalid.");
            return false;
        }

        // Iterate over the entities in the scene file
        const rapidjson::Value& objects = scene_document["objects"];
        for (rapidjson::SizeType i = 0; i < objects.Size(); ++i) {
            const rapidjson::Value& obj = objects[i];

            // Extract entity name
            std::string entity_name;
            if (obj.HasMember("name") && obj["name"].IsString()) {
                entity_name = obj["name"].GetString();
            }
            else {
                LM.write_log("Serialization_Manager::load_scene(): Entity at index %zu is missing 'name' or 'name' is not a string. Skipping.", i);
                continue;
            }

            // Create the entity
            EntityID eid = ECSM.create_entity();
            LM.write_log("Serialization_Manager::load_scene(): Created entity '%s' with ID %u.", entity_name.c_str(), eid);

            // Initialize merged components
            rapidjson::Document::AllocatorType& allocator = m_document.GetAllocator(); // Use the main document's allocator
            rapidjson::Value merged_components(rapidjson::kObjectType);

            // Handle prefab
            if (obj.HasMember("prefab") && obj["prefab"].IsString()) {
                std::string prefab_name = obj["prefab"].GetString();
                auto prefab_it = m_prefab_map.find(prefab_name);
                if (prefab_it != m_prefab_map.end()) {
                    const rapidjson::Value& prefab = prefab_it->second;
                    if (prefab.HasMember("components") && prefab["components"].IsObject()) {
                        merged_components.CopyFrom(prefab["components"], allocator);
                    }
                    else {
                        LM.write_log("Serialization_Manager::load_scene(): Prefab '%s' does not have 'components' object. Skipping entity '%s'.", prefab_name.c_str(), entity_name.c_str());
                        continue;
                    }
                }
                else {
                    LM.write_log("Serialization_Manager::load_scene(): Prefab '%s' not found. Skipping entity '%s'.", prefab_name.c_str(), entity_name.c_str());
                    continue;
                }
            }

            // Merge overrides from the scene
            if (obj.HasMember("components") && obj["components"].IsObject()) {
                const rapidjson::Value& scene_components = obj["components"];
                merge_objects(scene_components, merged_components, allocator);
            }

            // Add components to the entity using Component_Parser
            Component_Parser::add_components_from_json(ECSM, eid, merged_components);
        }

        LM.write_log("Serialization_Manager::load_scene(): Scene loaded successfully from %s.", filename);
        return true;
    }

    /**
     * @brief Retrieves the screen width from the configuration.
     *
     * @return Screen width as an unsigned integer.
     */
    unsigned int Serialization_Manager::get_scr_width() const {
        LM.write_log("Serialization_Manager::get_scr_width(): Returning SCR_WIDTH: %u", m_scr_width);
        return m_scr_width;
    }

    /**
     * @brief Retrieves the screen height from the configuration.
     *
     * @return Screen height as an unsigned integer.
     */
    unsigned int Serialization_Manager::get_scr_height() const {
        LM.write_log("Serialization_Manager::get_scr_height(): Returning SCR_HEIGHT: %u", m_scr_height);
        return m_scr_height;
    }

    /**
     * @brief Retrieves the FPS display interval from the configuration.
     *
     * @return FPS display interval as a float.
     */
    float Serialization_Manager::get_fps_display_interval() const {
        LM.write_log("Serialization_Manager::get_fps_display_interval(): Returning FPS_DISPLAY_INTERVAL: %.2f", m_fps_display_interval);
        return m_fps_display_interval;
    }

    /**
     * @brief Retrieves a prefab's JSON configuration by its name.
     *
     * @param prefab_name The name of the prefab to retrieve.
     * @return A constant pointer to the prefab's JSON value if found; otherwise, nullptr.
     */
    const rapidjson::Value* Serialization_Manager::get_prefab(const std::string& prefab_name) const {
        auto it = m_prefab_map.find(prefab_name);
        if (it != m_prefab_map.end()) {
            LM.write_log("Serialization_Manager::get_prefab(): Found prefab '%s'.", prefab_name.c_str());
            return &(it->second);
        }
        LM.write_log("Serialization_Manager::get_prefab(): Prefab '%s' not found.", prefab_name.c_str());
        return nullptr;
    }

} // namespace lof
