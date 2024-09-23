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

// Include Model class
#include "../Utility/Model.h"

// Include standard headers
#include <fstream>
#include <sstream>
#include <windows.h> // For GetModuleFileNameA

namespace {
    /**
     * @brief Retrieves the directory of the executable.
     * @return The executable's directory as a string.
     */
    std::string GetExecutableDirectory() {
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

        // Load objects configuration
        if (!load_object("..\\..\\lack_of_oxygen\\Data\\object.json")) {
            LM.write_log("Failed to load objects configuration file.");
            return -1; // Error loading objects configuration
        }

        return 0;
    }

    void Serialization_Manager::shut_down() {
        m_document.SetNull();           // Clear the document
        m_models.clear();               // Clear loaded models
        m_entities_config.clear();      // Clear entity configurations
        m_is_started = false;
    }

    bool Serialization_Manager::load_config(const std::string& filename) {
        // Log the full path being used
        std::string fullPath = GetExecutableDirectory() + "\\" + filename;
        LM.write_log("Attempting to load configuration file from: %s", fullPath.c_str());

        // Read the JSON file into a string
        std::ifstream ifs(fullPath);
        if (!ifs.is_open()) {
            LM.write_log("Failed to open configuration file: %s", fullPath.c_str());
            return false;
        }

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string json_content = buffer.str();

        // Parse the JSON content
        m_document.Parse(json_content.c_str());

        if (m_document.HasParseError()) {
            size_t offset = m_document.GetErrorOffset();
            rapidjson::ParseErrorCode errorCode = m_document.GetParseError();
            LM.write_log("JSON parse error at offset %zu: %s", offset, rapidjson::GetParseError_En(errorCode));
            return false;
        }

        if (!m_document.IsObject()) {
            LM.write_log("Invalid JSON format: Root element is not an object.");
            return false;
        }

        // Retrieve and store the configuration values
        // SCR_WIDTH
        if (m_document.HasMember("SCR_WIDTH") && m_document["SCR_WIDTH"].IsUint()) {
            m_scr_width = m_document["SCR_WIDTH"].GetUint();
            LM.write_log("Loaded SCR_WIDTH: %u", m_scr_width);
        }
        else {
            LM.write_log("SCR_WIDTH is missing or not an unsigned integer. Using default value: %u", m_scr_width);
        }

        // SCR_HEIGHT
        if (m_document.HasMember("SCR_HEIGHT") && m_document["SCR_HEIGHT"].IsUint()) {
            m_scr_height = m_document["SCR_HEIGHT"].GetUint();
            LM.write_log("Loaded SCR_HEIGHT: %u", m_scr_height);
        }
        else {
            LM.write_log("SCR_HEIGHT is missing or not an unsigned integer. Using default value: %u", m_scr_height);
        }

        // FPS_DISPLAY_INTERVAL
        if (m_document.HasMember("FPS_DISPLAY_INTERVAL") && m_document["FPS_DISPLAY_INTERVAL"].IsNumber()) {
            m_fps_display_interval = m_document["FPS_DISPLAY_INTERVAL"].GetFloat();
            LM.write_log("Loaded FPS_DISPLAY_INTERVAL: %.2f", m_fps_display_interval);
        }
        else {
            LM.write_log("FPS_DISPLAY_INTERVAL is missing or not a number. Using default value: %.2f", m_fps_display_interval);
        }

        // Since DATA_DIRECTORY is set in the constructor, we'll log it
        LM.write_log("Using DATA_DIRECTORY: %s", m_data_directory.c_str());

        LM.write_log("Configuration loaded successfully from %s.", fullPath.c_str());
        return true;
    }

    bool Serialization_Manager::load_mesh(const std::string& filename) {
        // Extract model name without extension for mapping
        size_t last_dot = filename.find_last_of('.');
        std::string model_name = (last_dot != std::string::npos) ? filename.substr(0, last_dot) : filename;

        // Check if the model is already loaded
        auto it = m_models.find(model_name);
        if (it != m_models.end()) {
            LM.write_log("Mesh '%s' already loaded. Skipping reloading.", model_name.c_str());
            return true; // Already loaded
        }

        // Construct full path using m_data_directory
        std::string fullPath = GetExecutableDirectory() + "\\" + m_data_directory + "\\" + filename;
        LM.write_log("Attempting to load mesh file from: %s", fullPath.c_str());

        // Create a new Model instance and load the mesh
        std::shared_ptr<Model> model = std::make_shared<Model>();
        if (!model->load_from_file(fullPath)) {
            LM.write_log("Failed to load mesh file: %s", fullPath.c_str());
            return false;
        }

        // Store the loaded model in the map
        m_models[model_name] = model;
        LM.write_log("Successfully loaded and stored mesh '%s'.", model_name.c_str());
        return true;
    }

    bool Serialization_Manager::load_object(const std::string& filename) {
        // Log the full path being used
        std::string fullPath = GetExecutableDirectory() + "\\" + filename;
        LM.write_log("Attempting to load objects file from: %s", fullPath.c_str());

        // Read the JSON file into a string
        std::ifstream ifs(fullPath);
        if (!ifs.is_open()) {
            LM.write_log("Failed to open object.json file: %s", fullPath.c_str());
            return false;
        }

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string json_content = buffer.str();

        // Parse the JSON content
        rapidjson::Document doc;
        doc.Parse(json_content.c_str());

        if (doc.HasParseError()) {
            size_t offset = doc.GetErrorOffset();
            rapidjson::ParseErrorCode errorCode = doc.GetParseError();
            LM.write_log("JSON parse error in object.json at offset %zu: %s", offset, rapidjson::GetParseError_En(errorCode));
            return false;
        }

        if (!doc.IsObject()) {
            LM.write_log("Invalid JSON format: Root element is not an object in object.json.");
            return false;
        }

        // Adjusted to use "object" as the root key
        if (!doc.HasMember("object") || !doc["object"].IsArray()) {
            LM.write_log("Invalid JSON format: 'object' array is missing or not an array.");
            return false;
        }

        const rapidjson::Value& objects = doc["object"];

        for (rapidjson::SizeType i = 0; i < objects.Size(); ++i) {
            const rapidjson::Value& obj = objects[i];
            if (!obj.IsObject()) {
                LM.write_log("Serialization_Manager::load_object(): Invalid object format at index %zu in object.json.", i);
                continue;
            }

            EntityConfig entity_config;

            // Extract object ID
            if (!obj.HasMember("id") || !obj["id"].IsUint()) {
                LM.write_log("Serialization_Manager::load_object(): Missing or invalid 'id' for object at index %zu.", i);
                continue;
            }
            entity_config.id = obj["id"].GetUint();

            // Extract object name (optional)
            if (obj.HasMember("name") && obj["name"].IsString()) {
                entity_config.name = obj["name"].GetString();
            }
            else {
                entity_config.name = "Unnamed";
            }

            // Log object information
            LM.write_log("Serialization_Manager::load_object(): Parsed object ID: %u, Name: %s",
                entity_config.id, entity_config.name.c_str());

            // Extract components
            if (!obj.HasMember("components") || !obj["components"].IsObject()) {
                LM.write_log("Serialization_Manager::load_object(): Missing or invalid 'components' for entity ID %u.", entity_config.id);
                continue;
            }

            const rapidjson::Value& components = obj["components"];

            // Iterate over components
            for (auto itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
                if (!itr->value.IsObject()) {
                    LM.write_log("Serialization_Manager::load_object(): Invalid component format for '%s' in entity ID %u.",
                        itr->name.GetString(), entity_config.id);
                    continue;
                }

                ComponentConfig comp_config;
                comp_config.type = itr->name.GetString();

                // **Use m_document's allocator instead of doc's allocator**
                comp_config.properties.CopyFrom(itr->value, m_document.GetAllocator());

                // Add comp_config to entity_config
                entity_config.components.push_back(std::move(comp_config));
            }

            // Extract model filename from Model_Component
            std::string model_filename = "default.msh"; // Default mesh
            for (const auto& comp : entity_config.components) {
                if (comp.type == "Model_Component") {
                    if (comp.properties.HasMember("model_name") && comp.properties["model_name"].IsString()) {
                        model_filename = comp.properties["model_name"].GetString();
                    }
                    break; // Assuming only one Model_Component
                }
            }

            // Log model information
            LM.write_log("Serialization_Manager::load_object(): Model filename for entity ID %u: %s",
                entity_config.id, model_filename.c_str());

            // Call load_mesh before moving entity_config
            if (!load_mesh(model_filename)) {
                LM.write_log("Serialization_Manager::load_object(): Failed to load mesh '%s' for entity ID %u.",
                    model_filename.c_str(), entity_config.id);
                // Optionally, handle the failure (e.g., continue or return false)
            }

            // Now, move entity_config into the vector
            m_entities_config.push_back(std::move(entity_config));
        }

        LM.write_log("Serialization_Manager::load_object(): Successfully parsed objects from object.json.");
        return true;
    }

    // Getter methods
    unsigned int Serialization_Manager::get_scr_width() const {
        return m_scr_width;
    }

    unsigned int Serialization_Manager::get_scr_height() const {
        return m_scr_height;
    }

    float Serialization_Manager::get_fps_display_interval() const {
        return m_fps_display_interval;
    }

    const std::vector<EntityConfig>& Serialization_Manager::get_entities_config() const {
        return m_entities_config;
    }

    const std::unordered_map<std::string, std::shared_ptr<Model>>& Serialization_Manager::get_models() const {
        return m_models;
    }

    std::shared_ptr<Model> Serialization_Manager::get_model(const std::string& model_name) const {
        auto it = m_models.find(model_name);
        if (it != m_models.end()) {
            return it->second;
        }
        return nullptr;
    }

} // namespace lof
