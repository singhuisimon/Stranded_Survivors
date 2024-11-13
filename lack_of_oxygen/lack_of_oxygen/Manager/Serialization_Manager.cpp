/**
 * @file Serialization_Manager.cpp
 * @brief Implements the Serialization_Manager class methods.
 * @author Simon Chan (99.581%), Liliana Hanawardani (0.418%)
 * @date September 22, 2024 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "Serialization_Manager.h"

// Include RapidJSON headers
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"    // For StringBuffer
#include "rapidjson/prettywriter.h"    // For PrettyWriter
#include "rapidjson/writer.h"          // For Writer

// Include Log_Manager
#include "Log_Manager.h"

// Include ECS_Manager for entity creation
#include "ECS_Manager.h"
#include "IMGUI_Manager.h"

// Include all component headers
#include "../Component/Component.h"

// Include Utility headers
#include "../Utility/Matrix3x3.h"
#include "../Utility/Component_Parser.h" // Adding components from JSON
#include "../Utility/Force_Helper.h"
#include "../Utility/Constant.h"
//#include "../Utility/Path_Helper.h"
#include "../Utility/Force_Helper.h"
// #include "../Utility/Path_Helper.h"

// Include standard headers
#include <fstream>
#include <sstream>
#include <cassert>

namespace lof {

    // Forward declaration of helper functions
    class Transform2D;
    class Graphics_Component;
    class Collision_Component;

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

        // Load general configuration using Path_Helper
        const std::string CONFIG = "Config";
        std::string config_path = ASM.get_full_path(CONFIG, "config.json");
        if (!load_config(config_path.c_str())) {
            LM.write_log("Serialization_Manager::start_up(): Failed to load game configuration file: %s", config_path.c_str());
            return -1;
        }

        // Load prefabs using Path_Helper
        const std::string PREFABS = "Prefab";
        std::string prefabs_path = ASM.get_full_path(PREFABS, "prefab.json");
        if (!load_prefabs(prefabs_path.c_str())) {
            LM.write_log("Serialization_Manager::start_up(): Failed to load prefab file: %s", prefabs_path.c_str());
            return -2;
        }

        // Load scene file using Path_Helper
        const std::string SCENES = "Scenes";
        std::string scene_path = ASM.get_full_path(SCENES, "scene1.scn");
        if (!load_scene(scene_path.c_str())) {
            LM.write_log("Serialization_Manager::start_up(): Failed to load scene file: %s", scene_path.c_str());
            return -3;
        }

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
        assert(source.IsObject());
        assert(destination.IsObject());

        for (auto itr = source.MemberBegin(); itr != source.MemberEnd(); ++itr) {
            const char* key = itr->name.GetString();

            if (destination.HasMember(key)) {
                // If both values are objects, merge them recursively
                if (itr->value.IsObject() && destination[key].IsObject()) {
                    LM.write_log("Serialization_Manager::merge_objects(): Merging object at key '%s'.", key);
                    merge_objects(itr->value, destination[key], allocator);
                }
                else {
                    // Overwrite the value in the destination
                    LM.write_log("Serialization_Manager::merge_objects(): Overwriting key '%s'.", key);
                    destination[key].CopyFrom(itr->value, allocator);
                }
            }
            else {
                // Add the new key-value pair to the destination
                LM.write_log("Serialization_Manager::merge_objects(): Adding key '%s'.", key);
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
            
            IMGUIM.fill_prefab_names(prefab_name.c_str());

            //DEBUG
            LM.write_log("DEBUG: Attempting to load prefab: %s", prefab_name.c_str());

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

        // Clear all existing entities first
        const auto& entities = ECSM.get_entities();
        std::vector<EntityID> entities_to_remove;

        // Collect all existing entity IDs
        for (size_t i = 0; i < entities.size(); ++i) {
            if (entities[i]) {  // Check if entity exists
                entities_to_remove.push_back(static_cast<EntityID>(i));
            }
        }

        // Remove all existing entities
        for (EntityID eid : entities_to_remove) {
            ECSM.destroy_entity(0);
        }

        LM.write_log("Serialization_Manager::load_scene(): Cleared %zu existing entities.", entities_to_remove.size());

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
                LM.write_log("Serialization_Manager::load_scene(): Entity at index %zu is missing 'name' or 'name' is not a string. Using default name.", i);
                entity_name = "unnamed_entity_" + std::to_string(i);
            }

            EntityID eid;
            // Handle prefab-based entities
            if (obj.HasMember("prefab") && obj["prefab"].IsString()) {
                std::string prefab_name = obj["prefab"].GetString();
                eid = ECSM.clone_entity_from_prefab(prefab_name, entity_name);
                if (eid == INVALID_ENTITY_ID) {
                    LM.write_log("Serialization_Manager::load_scene(): Failed to create entity from prefab '%s'. Skipping.", prefab_name.c_str());
                    continue;
                }
            }
            else {
                // Create a new entity without prefab
                eid = ECSM.create_entity(entity_name);
            }

            // Set the entity's name
            if (auto* entity = ECSM.get_entity(eid)) {
                entity->set_name(entity_name);
                LM.write_log("Serialization_Manager::load_scene(): Created entity '%s' with ID %u.", entity_name.c_str(), eid);
            }

            // Initialize merged components
            rapidjson::Document::AllocatorType& allocator = m_document.GetAllocator();
            rapidjson::Value merged_components(rapidjson::kObjectType);

            // If this is a prefab-based entity, start with the prefab's components
            if (obj.HasMember("prefab") && obj["prefab"].IsString()) {
                std::string prefab_name = obj["prefab"].GetString();
                auto prefab_it = m_prefab_map.find(prefab_name);
                if (prefab_it != m_prefab_map.end()) {
                    const rapidjson::Value& prefab = prefab_it->second;
                    if (prefab.HasMember("components") && prefab["components"].IsObject()) {
                        merged_components.CopyFrom(prefab["components"], allocator);
                    }
                }
            }

            // Merge components from the scene (overrides prefab components)
            if (obj.HasMember("components") && obj["components"].IsObject()) {
                const rapidjson::Value& scene_components = obj["components"];
                merge_objects(scene_components, merged_components, allocator);
            }

            // Add components to the entity
            Component_Parser::add_components_from_json(ECSM, eid, merged_components);
        }

        LM.write_log("Serialization_Manager::load_scene(): Scene loaded successfully from %s.", filename);
        return true;
    }

    rapidjson::Value Serialization_Manager::serialize_transform_component(const Transform2D& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        // Position
        rapidjson::Value position(rapidjson::kArrayType);
        position.PushBack(component.position.x, allocator);
        position.PushBack(component.position.y, allocator);
        comp_obj.AddMember("position", position, allocator);

        // Previous position (matching your scene format)
        rapidjson::Value prev_position(rapidjson::kArrayType);
        prev_position.PushBack(component.position.x, allocator);
        prev_position.PushBack(component.position.y, allocator);
        comp_obj.AddMember("prev_position", prev_position, allocator);

        // Orientation
        rapidjson::Value orientation(rapidjson::kArrayType);
        orientation.PushBack(component.orientation.x, allocator);
        orientation.PushBack(component.orientation.y, allocator);
        comp_obj.AddMember("orientation", orientation, allocator);

        // Scale
        rapidjson::Value scale(rapidjson::kArrayType);
        scale.PushBack(component.scale.x, allocator);
        scale.PushBack(component.scale.y, allocator);
        comp_obj.AddMember("scale", scale, allocator);

        return comp_obj;
    }


    rapidjson::Value Serialization_Manager::serialize_graphics_component(const Graphics_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        comp_obj.AddMember("model_name", rapidjson::Value(component.model_name.c_str(), allocator), allocator);

        rapidjson::Value color(rapidjson::kArrayType);
        color.PushBack(component.color.x, allocator);
        color.PushBack(component.color.y, allocator);
        color.PushBack(component.color.z, allocator);
        comp_obj.AddMember("color", color, allocator);

        comp_obj.AddMember("texture_name", rapidjson::Value(component.texture_name.c_str(), allocator), allocator);
        comp_obj.AddMember("shd_ref", component.shd_ref, allocator);

        // Matrix in your scene file format
        rapidjson::Value matrix(rapidjson::kArrayType);
        for (int i = 0; i < 3; ++i) {
            rapidjson::Value row(rapidjson::kArrayType);
            for (int j = 0; j < 3; ++j) {
                row.PushBack(component.mdl_to_ndc_xform[i][j], allocator);
            }
            matrix.PushBack(row, allocator);
        }
        comp_obj.AddMember("mdl_to_ndc_xform", matrix, allocator);

        return comp_obj;
    }


    rapidjson::Value Serialization_Manager::serialize_collision_component(const Collision_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        comp_obj.AddMember("width", component.width, allocator);
        comp_obj.AddMember("height", component.height, allocator);

        return comp_obj;
    }

    rapidjson::Value Serialization_Manager::serialize_physics_component(const Physics_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        // Serialize gravity vector
        Vec2D gravity = component.get_gravity();
        rapidjson::Value gravity_arr(rapidjson::kArrayType);
        gravity_arr.PushBack(gravity.x, allocator);
        gravity_arr.PushBack(gravity.y, allocator);
        comp_obj.AddMember("gravity", gravity_arr, allocator);

        // Serialize scalar physics properties
        comp_obj.AddMember("damping_factor", component.get_damping_factor(), allocator);
        comp_obj.AddMember("max_velocity", component.get_max_velocity(), allocator);
        comp_obj.AddMember("mass", component.get_mass(), allocator);
        comp_obj.AddMember("jump_force", component.get_jump_force(), allocator);

        // Serialize boolean flags
        comp_obj.AddMember("is_static", component.get_is_static(), allocator);
        comp_obj.AddMember("is_grounded", component.get_is_grounded(), allocator);
        comp_obj.AddMember("has_jumped", component.get_has_jumped(), allocator);
        comp_obj.AddMember("jump_requested", component.get_jump_requested(), allocator);


        // Serialize accumulated force
        Vec2D acc_force = component.get_accumulated_force();
        rapidjson::Value accumulated_force(rapidjson::kArrayType);
        accumulated_force.PushBack(acc_force.x, allocator);
        accumulated_force.PushBack(acc_force.y, allocator);
        comp_obj.AddMember("accumulated_force", accumulated_force, allocator);

        //serialize force_helper
        rapidjson::Value force_helper_obj(rapidjson::kObjectType);
        rapidjson::Value forces_array(rapidjson::kArrayType);

        //get all forces from force manager
        const auto& forces = component.force_helper.get_forces();

        for (const auto& force : forces) {
            rapidjson::Value force_obj(rapidjson::kObjectType);

            // Serialize direction
            rapidjson::Value direction(rapidjson::kArrayType);
            direction.PushBack(force.direction.x, allocator);
            direction.PushBack(force.direction.y, allocator);
            force_obj.AddMember("direction", direction, allocator);

            std::string type_str = Force::ftype_to_string(force.type);
            force_obj.AddMember("type", rapidjson::Value(type_str.c_str(), allocator), allocator);
            // Serialize other force properties
            force_obj.AddMember("magnitude", force.magnitude, allocator);
            force_obj.AddMember("lifetime", force.lifetime, allocator);
            force_obj.AddMember("is_active", force.is_active, allocator);

            forces_array.PushBack(force_obj, allocator);
        }
        force_helper_obj.AddMember("forces", forces_array, allocator);
        comp_obj.AddMember("force_helper", force_helper_obj, allocator);

        return comp_obj;
    }

    rapidjson::Value Serialization_Manager::serialize_velocity_component(const Velocity_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        // Serialize velocity vector
        rapidjson::Value velocity(rapidjson::kArrayType);
        velocity.PushBack(component.velocity.x, allocator);
        velocity.PushBack(component.velocity.y, allocator);
        comp_obj.AddMember("velocity", velocity, allocator);

        return comp_obj;
    }

    rapidjson::Value Serialization_Manager::serialize_audio_component(const Audio_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        // Create the sounds array
        rapidjson::Value sounds_array(rapidjson::kArrayType);
        auto& all_sounds = component.get_sounds();

        for (auto& sound : all_sounds) {
            rapidjson::Value sound_obj(rapidjson::kObjectType);

            // Add key
            sound_obj.AddMember("key", rapidjson::Value(sound.key.c_str(), allocator), allocator);

            // Handle filepath - extract just the basic filename without path
            std::string filename = sound.filepath;
            size_t last_slash = filename.find_last_of("/\\");
            if (last_slash != std::string::npos) {
                filename = filename.substr(last_slash + 1);
            }
            // Remove the .wav extension if present
            size_t extension = filename.find(".wav");
            if (extension != std::string::npos) {
                filename = filename.substr(0, extension);
            }
            sound_obj.AddMember("filepath", rapidjson::Value(filename.c_str(), allocator), allocator);

            // Add other sound properties
            sound_obj.AddMember("audio_state", static_cast<int>(sound.audio_state), allocator);
            sound_obj.AddMember("audio_type", static_cast<int>(sound.audio_type), allocator);
            sound_obj.AddMember("volume", sound.volume, allocator);
            sound_obj.AddMember("pitch", sound.pitch, allocator);
            // Use "islooping" to match original format
            sound_obj.AddMember("islooping", sound.islooping, allocator);

            // Add the serialized sound object to the sounds array
            sounds_array.PushBack(sound_obj, allocator);
        }

        comp_obj.AddMember("sounds", sounds_array, allocator);

        // Add other audio component properties
        comp_obj.AddMember("is_3d", component.get_is3d(), allocator);

        rapidjson::Value position(rapidjson::kArrayType);
        Vec3D pos = component.get_position();
        position.PushBack(pos.x, allocator);
        position.PushBack(pos.y, allocator);
        position.PushBack(pos.z, allocator);
        comp_obj.AddMember("position", position, allocator);

        comp_obj.AddMember("min_distance", component.get_min_distance(), allocator);
        comp_obj.AddMember("max_distance", component.get_max_distance(), allocator);

        return comp_obj;
    }

    rapidjson::Value Serialization_Manager::serialize_animation_component(const Animation_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        // Create animations array
        rapidjson::Value animations_array(rapidjson::kArrayType);

        // Iterate through the animations map
        for (const auto& [index, name] : component.animations) {
            // Create array for each animation entry
            rapidjson::Value animation_entry(rapidjson::kArrayType);

            // Add index and name as separate array elements
            animation_entry.PushBack(rapidjson::Value(index.c_str(), allocator), allocator);
            animation_entry.PushBack(rapidjson::Value(name.c_str(), allocator), allocator);

            // Add the entry to animations array
            animations_array.PushBack(animation_entry, allocator);
        }

        // Add animations array to component object
        comp_obj.AddMember("animations", animations_array, allocator);

        // Add other animation component properties
        comp_obj.AddMember("curr_animation_idx", component.curr_animation_idx, allocator);
        comp_obj.AddMember("start_animation_idx", component.start_animation_idx, allocator);

        return comp_obj;
    }

    rapidjson::Value Serialization_Manager::serialize_logic_component(const Logic_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        // Add primitive members with explicit rapidjson::Value creation
        comp_obj.AddMember("logic_type", rapidjson::Value(static_cast<int>(component.logic_type)), allocator);
        comp_obj.AddMember("movement_pattern", rapidjson::Value(static_cast<int>(component.movement_pattern)), allocator);
        comp_obj.AddMember("is_active", rapidjson::Value(component.is_active), allocator);
        comp_obj.AddMember("movement_speed", rapidjson::Value(component.movement_speed), allocator);
        comp_obj.AddMember("movement_range", rapidjson::Value(component.movement_range), allocator);
        comp_obj.AddMember("reverse_direction", rapidjson::Value(component.reverse_direction), allocator);
        comp_obj.AddMember("rotate_with_motion", rapidjson::Value(component.rotate_with_motion), allocator);

        // Add origin position as array
        rapidjson::Value origin_pos(rapidjson::kArrayType);
        origin_pos.PushBack(rapidjson::Value(component.origin_pos.x), allocator);
        origin_pos.PushBack(rapidjson::Value(component.origin_pos.y), allocator);
        comp_obj.AddMember("origin_pos", origin_pos, allocator);

        return comp_obj;
    }

    rapidjson::Value Serialization_Manager::serialize_text_component(const Text_Component& component, rapidjson::Document::AllocatorType& allocator) {
        rapidjson::Value comp_obj(rapidjson::kObjectType);

        // Add font name and text 
        comp_obj.AddMember("font_name", rapidjson::Value(component.font_name.c_str(), allocator), allocator);
        comp_obj.AddMember("text", rapidjson::Value(component.text.c_str(), allocator), allocator);

        // Add color
        rapidjson::Value color(rapidjson::kArrayType);
        color.PushBack(component.color.x, allocator);
        color.PushBack(component.color.y, allocator);
        color.PushBack(component.color.z, allocator);
        comp_obj.AddMember("color", color, allocator);

        return comp_obj;
    }

    bool Serialization_Manager::save_game_state(const char* filepath) {
        LM.write_log("Serialization_Manager::save_game_state(): Starting to save game state to %s", filepath);
        rapidjson::Document save_doc;
        save_doc.SetObject();
        rapidjson::Document::AllocatorType& allocator = save_doc.GetAllocator();

        // Create array for objects (matching your scene format)
        rapidjson::Value objects_array(rapidjson::kArrayType);

        // Counter for generating unique names if needed
        int unnamed_counter = 0;

        // Iterate through all entities
        for (const auto& entity_ptr : ECSM.get_entities()) {
            EntityID entity_id = entity_ptr->get_id();

            // Skip if this entity only has a GUI component
            if (entity_ptr->get_component_mask().count() == 1 &&
                ECSM.has_component<GUI_Component>(entity_id)) {
                continue;
            }

            // Create object for this entity
            rapidjson::Value entity_obj(rapidjson::kObjectType);

            // Generate a name for the entity
            std::string entity_name;
            if (entity_id == 0) {
                entity_name = "background";
            }
            else {
                entity_name = "entity_" + std::to_string(unnamed_counter++);
            }
            entity_obj.AddMember("name", rapidjson::Value(entity_name.c_str(), allocator), allocator);

            // Create object for components
            rapidjson::Value components_obj(rapidjson::kObjectType);

            // Add Transform2D component if present
            if (ECSM.has_component<Transform2D>(entity_id)) {
                const Transform2D& transform = ECSM.get_component<Transform2D>(entity_id);
                components_obj.AddMember("Transform2D", serialize_transform_component(transform, allocator), allocator);
            }

            // Add Graphics_Component if present
            if (ECSM.has_component<Graphics_Component>(entity_id)) {
                const Graphics_Component& graphics = ECSM.get_component<Graphics_Component>(entity_id);
                components_obj.AddMember("Graphics_Component", serialize_graphics_component(graphics, allocator), allocator);
            }

            // Add Collision_Component if present
            if (ECSM.has_component<Collision_Component>(entity_id)) {
                const Collision_Component& collision = ECSM.get_component<Collision_Component>(entity_id);
                components_obj.AddMember("Collision_Component", serialize_collision_component(collision, allocator), allocator);
            }

            // Add Physics_Component if present
            if (ECSM.has_component<Physics_Component>(entity_id)) {
                const Physics_Component& physics = ECSM.get_component<Physics_Component>(entity_id);
                components_obj.AddMember("Physics_Component", serialize_physics_component(physics, allocator), allocator);
            }

            // Add Velocity_Component if present
            if (ECSM.has_component<Velocity_Component>(entity_id)) {
                const Velocity_Component& velocity = ECSM.get_component<Velocity_Component>(entity_id);
                components_obj.AddMember("Velocity_Component", serialize_velocity_component(velocity, allocator), allocator);
            }

            // Add Audio_Component if present
            if (ECSM.has_component<Audio_Component>(entity_id)) {
                const Audio_Component& audio = ECSM.get_component<Audio_Component>(entity_id);
                components_obj.AddMember("Audio_Component", serialize_audio_component(audio, allocator), allocator);
            }

            // Add Animation_Component if present
            if (ECSM.has_component<Animation_Component>(entity_id)) {
                const Animation_Component& animation = ECSM.get_component<Animation_Component>(entity_id);
                components_obj.AddMember("Animation_Component", serialize_animation_component(animation, allocator), allocator);
            }

            //// Add Text_Component if present
            //if (ECSM.has_component<Text_Component>(entity_id)) {
            //    const Text_Component& text = ECSM.get_component<Text_Component>(entity_id);
            //    components_obj.AddMember("Text_Component", serialize_text_component(text, allocator), allocator);
            //}

            // Only add entity to save file if it has components to save
            if (components_obj.MemberCount() > 0) {
                // Add components object to entity object
                entity_obj.AddMember("components", components_obj, allocator);
                // Add entity object to objects array
                objects_array.PushBack(entity_obj, allocator);
            }
        }

        // Add objects array to root object
        save_doc.AddMember("objects", objects_array, allocator);

        // Write to file
        try {
            std::ofstream ofs(filepath);
            if (!ofs.is_open()) {
                LM.write_log("Serialization_Manager::save_game_state(): Failed to open file for writing: %s", filepath);
                return false;
            }

            rapidjson::StringBuffer strbuf;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
            save_doc.Accept(writer);
            ofs << strbuf.GetString();
            ofs.close();

            LM.write_log("Serialization_Manager::save_game_state(): Successfully saved game state to %s", filepath);
            return true;
        }
        catch (const std::exception& e) {
            LM.write_log("Serialization_Manager::save_game_state(): Error writing save file: %s", e.what());
            return false;
        }
    }


    unsigned int Serialization_Manager::get_scr_width() const {
        //LM.write_log("Serialization_Manager::get_scr_width(): Returning SCR_WIDTH: %u", m_scr_width);
        return m_scr_width;
    }


    unsigned int Serialization_Manager::get_scr_height() const {
        //LM.write_log("Serialization_Manager::get_scr_height(): Returning SCR_HEIGHT: %u", m_scr_height);
        return m_scr_height;
    }


    float Serialization_Manager::get_fps_display_interval() const {
        LM.write_log("Serialization_Manager::get_fps_display_interval(): Returning FPS_DISPLAY_INTERVAL: %.2f", m_fps_display_interval);
        return m_fps_display_interval;
    }


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
