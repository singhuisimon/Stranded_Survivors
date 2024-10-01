/**
 * @file Serialization_Manager.cpp
 * @brief Implements the Serialization_Manager class methods.
 * @date September 21, 2024
 */

 // Include base headers
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

// Include math headers
#include "../Utility/Matrix3x3.h"

// Include Component_Parser for adding components from JSON
#include "../Utility/Component_Parser.h"

// Include standard headers
#include <fstream>
#include <sstream>
#include <windows.h> // For GetModuleFileNameA
#include <unordered_map>

namespace {
    /**
     * @brief Retrieves the directory of the executable.
     * @return The executable's directory as a string.
     */
    std::string get_executable_directory() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }
}

namespace lof {

    Serialization_Manager::Serialization_Manager()
        : m_scr_width(800),
        m_scr_height(600),
        m_fps_display_interval(1.0f),
        m_data_directory("..\\..\\lack_of_oxygen\\Data") { // Initialize m_data_directory
        set_type("Serialization_Manager");
        set_time(0);
    }

    Serialization_Manager& Serialization_Manager::get_instance() {
        static Serialization_Manager instance;
        return instance;
    }

    int Serialization_Manager::start_up() {
        m_is_started = true;

        // Load general configuration
        if (!load_config("..\\..\\lack_of_oxygen\\Data\\config.json")) {
            LM.write_log("Failed to load game configuration file.");
            return -1; // Error loading configuration file
        }

        // Load prefabs
        if (!load_prefabs("..\\..\\lack_of_oxygen\\Data\\prefab.json")) {
            LM.write_log("Failed to load prefab file.");
            return -2; // Error loading prefab file
        }

        // Load scene file
        if (!load_scene("..\\..\\lack_of_oxygen\\Data\\scene1.scn")) {
            LM.write_log("Failed to load scene file.");
            return -3; // Error loading scene file
        }

        return 0;
    }

    void Serialization_Manager::shut_down() {
        m_document.SetNull();           // Clear the document
        m_is_started = false;
    }

    void Serialization_Manager::merge_objects(const rapidjson::Value& source,
        rapidjson::Value& destination,
        rapidjson::Document::AllocatorType& allocator) {
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

    bool Serialization_Manager::load_config(const std::string& filename) {
        // Log the full path being used
        std::string full_path = get_executable_directory() + "\\" + filename;
        LM.write_log("Attempting to load configuration file from: %s", full_path.c_str());

        // Read the JSON file into a string
        std::ifstream ifs(full_path);
        if (!ifs.is_open()) {
            LM.write_log("Failed to open configuration file: %s", full_path.c_str());
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
            LM.write_log("JSON parse error at offset %zu: %s", offset, rapidjson::GetParseError_En(error_code));
            return false;
        }

        if (!m_document.IsObject()) {
            LM.write_log("Invalid JSON format: Root element is not an object.");
            return false;
        }

        // Retrieve and store the configuration values
        if (m_document.HasMember("SCR_WIDTH") && m_document["SCR_WIDTH"].IsUint()) {
            m_scr_width = m_document["SCR_WIDTH"].GetUint();
            LM.write_log("Loaded SCR_WIDTH: %u", m_scr_width);
        }
        else {
            LM.write_log("SCR_WIDTH is missing or not an unsigned integer. Using default value: %u", m_scr_width);
        }

        if (m_document.HasMember("SCR_HEIGHT") && m_document["SCR_HEIGHT"].IsUint()) {
            m_scr_height = m_document["SCR_HEIGHT"].GetUint();
            LM.write_log("Loaded SCR_HEIGHT: %u", m_scr_height);
        }
        else {
            LM.write_log("SCR_HEIGHT is missing or not an unsigned integer. Using default value: %u", m_scr_height);
        }

        if (m_document.HasMember("FPS_DISPLAY_INTERVAL") && m_document["FPS_DISPLAY_INTERVAL"].IsNumber()) {
            m_fps_display_interval = m_document["FPS_DISPLAY_INTERVAL"].GetFloat();
            LM.write_log("Loaded FPS_DISPLAY_INTERVAL: %.2f", m_fps_display_interval);
        }
        else {
            LM.write_log("FPS_DISPLAY_INTERVAL is missing or not a number. Using default value: %.2f", m_fps_display_interval);
        }

        LM.write_log("Using DATA_DIRECTORY: %s", m_data_directory.c_str());
        LM.write_log("Configuration loaded successfully from %s.", full_path.c_str());
        return true;
    }

    bool Serialization_Manager::load_prefabs(const std::string& filename) {
        // Construct the full path to the prefab file
        std::string prefab_path = get_executable_directory() + "\\" + filename;
        LM.write_log("Attempting to load prefabs from: %s", prefab_path.c_str());

        // Read and parse the prefab file
        std::ifstream ifs(prefab_path);
        if (!ifs.is_open()) {
            LM.write_log("Failed to open prefab file: %s", prefab_path.c_str());
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
            LM.write_log("JSON parse error in prefab file at offset %zu: %s", offset, rapidjson::GetParseError_En(error_code));
            return false;
        }

        if (!prefab_document.IsObject() || !prefab_document.HasMember("prefabs") || !prefab_document["prefabs"].IsObject()) {
            LM.write_log("Invalid prefab file format: 'prefabs' object is missing or invalid.");
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
            LM.write_log("Loaded prefab '%s' into cache.", prefab_name.c_str());
        }

        return true;
    }

    bool Serialization_Manager::load_scene(const std::string& filename) {
        // Construct the full path to the scene file
        std::string full_path = get_executable_directory() + "\\" + filename;
        LM.write_log("Attempting to load scene file from: %s", full_path.c_str());

        // Read and parse the scene file
        std::ifstream ifs(full_path);
        if (!ifs.is_open()) {
            LM.write_log("Failed to open scene file: %s", full_path.c_str());
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
            LM.write_log("JSON parse error at offset %zu: %s", offset, rapidjson::GetParseError_En(error_code));
            return false;
        }

        if (!scene_document.IsObject() || !scene_document.HasMember("objects") || !scene_document["objects"].IsArray()) {
            LM.write_log("Invalid scene file format: 'objects' array is missing or invalid.");
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
                LM.write_log("Entity at index %u is missing 'name' or 'name' is not a string. Skipping.", i);
                continue;
            }

            // Create the entity
            EntityID eid = ECSM.create_entity();
            LM.write_log("Created entity '%s' with ID %u.", entity_name.c_str(), eid);

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
                        LM.write_log("Prefab '%s' does not have 'components' object. Skipping entity '%s'.", prefab_name.c_str(), entity_name.c_str());
                        continue;
                    }
                }
                else {
                    LM.write_log("Prefab '%s' not found. Skipping entity '%s'.", prefab_name.c_str(), entity_name.c_str());
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

        LM.write_log("Scene loaded successfully from %s.", full_path.c_str());
        return true;
    }

    // Getter functions
    unsigned int Serialization_Manager::get_scr_width() const {
        return m_scr_width;
    }

    unsigned int Serialization_Manager::get_scr_height() const {
        return m_scr_height;
    }

    float Serialization_Manager::get_fps_display_interval() const {
        return m_fps_display_interval;
    }

    const rapidjson::Value* Serialization_Manager::get_prefab(const std::string& prefab_name) const {
        auto it = m_prefab_map.find(prefab_name);
        if (it != m_prefab_map.end()) {
            return &(it->second);
        }
        return nullptr;
    }

} // namespace lof
