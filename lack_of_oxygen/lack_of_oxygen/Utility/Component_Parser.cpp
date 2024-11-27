/**
 * @file ComponentParser.cpp
 * @brief Implements the ComponentParser utility class methods.
 * @author Simon Chan (100%)
 * @date October 1, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
// Include header file
#include "Component_Parser.h"

// Include Managers
#include "../Manager/ECS_Manager.h"
#include "../Manager/Log_Manager.h"

// Include Components
#include "../Component/Component.h"

// Include other necessary headers
#include "../Utility/Matrix3x3.h"
//#include "../Utility/Path_Helper.h"
#include "../Manager/Assets_Manager.h"

namespace lof {

    void Component_Parser::add_components_from_json(ECS_Manager& ecs_manager, EntityID entity, const rapidjson::Value& components) {
        for (auto it = components.MemberBegin(); it != components.MemberEnd(); ++it) {
            std::string component_name = it->name.GetString();
            const rapidjson::Value& component_data = it->value;

            // ------------------------------------ Transform2D -------------------------------------------
            if (component_name == "Transform2D") {
                // Parse Transform2D component
                Transform2D transform;
                if (component_data.HasMember("position") && component_data["position"].IsArray()) {
                    const rapidjson::Value& pos = component_data["position"];
                    transform.position.x = pos[0].GetFloat();
                    transform.position.y = pos[1].GetFloat();
                    transform.prev_position = transform.position;
                }
                if (component_data.HasMember("prev_position") && component_data["prev_position"].IsArray()) {
                    const rapidjson::Value& prev_pos = component_data["prev_position"];
                    transform.prev_position.x = prev_pos[0].GetFloat();
                    transform.prev_position.y = prev_pos[1].GetFloat();
                }
                else {
                    // Optional: Set a default value for prev_position if not provided
                    transform.prev_position = transform.position; // Or Vec2D(0.0f, 0.0f) as appropriate
                }
                if (component_data.HasMember("prev_position") && component_data["prev_position"].IsArray()) {
                    const rapidjson::Value& pos = component_data["prev_position"];
                    transform.prev_position.x = pos[0].GetFloat();
                    transform.prev_position.y = pos[1].GetFloat();
                }

                if (component_data.HasMember("orientation") && component_data["orientation"].IsArray()) {
                    const rapidjson::Value& ori = component_data["orientation"];
                    transform.orientation.x = ori[0].GetFloat();
                    transform.orientation.y = ori[1].GetFloat();
                }
                if (component_data.HasMember("scale") && component_data["scale"].IsArray()) {
                    const rapidjson::Value& scale = component_data["scale"];
                    transform.scale.x = scale[0].GetFloat();
                    transform.scale.y = scale[1].GetFloat();
                }

                // Add component to entity
                ecs_manager.add_component<Transform2D>(entity, transform);
                LM.write_log("Component_Parser::add_components_from_json(): Added Transform2D component to entity ID %u.", entity);
            }
            // ------------------------------------ Velocity_Component -------------------------------------------
            else if (component_name == "Velocity_Component") {
                // Parse Velocity_Component
                Velocity_Component velocity;
                if (component_data.HasMember("velocity") && component_data["velocity"].IsArray()) {
                    const rapidjson::Value& vel = component_data["velocity"];
                    velocity.velocity.x = vel[0].GetFloat();
                    velocity.velocity.y = vel[1].GetFloat();
                }

                // Add component to entity
                ecs_manager.add_component<Velocity_Component>(entity, velocity);
                LM.write_log("Component_Parser::add_components_from_json(): Added Velocity_Component to entity ID %u.", entity);
            }
            // ------------------------------------ Physics_Component -------------------------------------------
            else if (component_name == "Physics_Component") {
                // Parse Physics_Component
                Physics_Component physics_component;

                if (component_data.HasMember("gravity") && component_data["gravity"].IsArray()) {
                    const rapidjson::Value& grav = component_data["gravity"];
                    Vec2D gravity;
                    gravity.x = grav[0].GetFloat();
                    gravity.y = grav[1].GetFloat();

                    physics_component.set_gravity(gravity);
                }

                if (component_data.HasMember("damping_factor") && component_data["damping_factor"].IsNumber()) {
                    physics_component.set_damping_factor(component_data["damping_factor"].GetFloat());
                }

                if (component_data.HasMember("max_velocity") && component_data["max_velocity"].IsNumber()) {
                    physics_component.set_max_velocity(component_data["max_velocity"].GetFloat());
                }

                if (component_data.HasMember("accumulated_force") && component_data["accumulated_force"].IsArray()) {
                    const rapidjson::Value& acc_force = component_data["accumulated_force"];
                    Vec2D accumulated_force;
                    accumulated_force.x = acc_force[0].GetFloat();
                    accumulated_force.y = acc_force[1].GetFloat();

                    physics_component.set_accumulated_force(accumulated_force);
                }

                if (component_data.HasMember("mass") && component_data["mass"].IsNumber()) {
                    physics_component.set_mass(component_data["mass"].GetFloat());
                }

                if (component_data.HasMember("is_static") && component_data["is_static"].IsBool()) {
                    physics_component.set_is_static(component_data["is_static"].GetBool());
                }

                if (component_data.HasMember("is_grounded") && component_data["is_grounded"].IsBool()) {
                    physics_component.set_is_grounded(component_data["is_grounded"].GetBool());
                }

                if (component_data.HasMember("has_jumped") && component_data["has_jumped"].IsBool()) {
                    physics_component.set_is_grounded(component_data["has_jumped"].GetBool());
                }

                if (component_data.HasMember("jump_requested") && component_data["jump_requested"].IsBool()) {
                    physics_component.set_jump_requested(component_data["jump_requested"].GetBool());
                }

                if (component_data.HasMember("jump_force") && component_data["jump_force"].IsNumber()) {
                    physics_component.set_jump_force(component_data["jump_force"].GetFloat());
                }

                if (component_data.HasMember("force_helper") && component_data["force_helper"].IsObject()) {
                    const rapidjson::Value& force_helper = component_data["force_helper"];


                    if (force_helper.HasMember("forces") && force_helper["forces"].IsArray()) {

                        physics_component.force_helper = Force_Helper();

                        const rapidjson::Value& forces_array = force_helper["forces"];

                        for (const auto& force : forces_array.GetArray()) {
                            //parse direction
                            Vec2D direction(0.0f, 0.0f);
                            if (force.HasMember("direction") && force["direction"].IsArray()) {
                                const rapidjson::Value& dir = force["direction"];
                                direction.x = dir[0].GetFloat();
                                direction.y = dir[1].GetFloat();
                            }

                            // Parse force type 
                            ForceType type;
                            if (force.HasMember("type") && force["type"].IsString()) {
                                std::string type_str = force["type"].GetString();
                                type = Force::string_to_ftype(type_str);
                            }
                            //magnitude
                            float magnitude = 0.0f;
                            if (force.HasMember("magnitude") && force["magnitude"].IsNumber()) {
                                magnitude = force["magnitude"].GetFloat();
                            }
                            //lifetime
                            float lifetime = 0.0f;
                            if (force.HasMember("lifetime") && force["lifetime"].IsNumber()) {
                                lifetime = force["lifetime"].GetFloat();
                            }

                            //create and add the force type 
                            Force force_obj(direction, type, magnitude, lifetime);

                            //active state 
                            if (force.HasMember("is_active") && force["is_active"].IsBool()) {
                                force_obj.set_active(force["is_active"].GetBool());
                            }

                            physics_component.force_helper.add_force(force_obj);

                            LM.write_log("Component_Parser::add_components_from_json(): Added force of type %d to entity ID %u",
                                type, entity);

                        }
                    }

                }


                // Add component to entity
                ecs_manager.add_component<Physics_Component>(entity, physics_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Physics_Component to entity ID %u.", entity);
            }
            // ------------------------------------ Graphics_Component -------------------------------------------
            else if (component_name == "Graphics_Component") {
                // Parse Graphics_Component
                Graphics_Component graphics_component;

                if (component_data.HasMember("model_name") && component_data["model_name"].IsString()) {
                    graphics_component.model_name = component_data["model_name"].GetString();
                }
                else {
                    graphics_component.model_name = DEFAULT_MODEL_NAME; // Default value
                }

                if (component_data.HasMember("color") && component_data["color"].IsArray()) {
                    const rapidjson::Value& clr = component_data["color"];
                    graphics_component.color.x = clr[0].GetFloat();
                    graphics_component.color.y = clr[1].GetFloat();
                    graphics_component.color.z = clr[2].GetFloat();
                }

                if (component_data.HasMember("texture_name") && component_data["texture_name"].IsString()) {
                    graphics_component.texture_name = component_data["texture_name"].GetString();
                }
                else {
                    graphics_component.texture_name = DEFAULT_TEXTURE_NAME; // Default value
                }

                if (component_data.HasMember("shd_ref") && component_data["shd_ref"].IsUint()) {
                    graphics_component.shd_ref = component_data["shd_ref"].GetUint();
                }
                else {
                    graphics_component.shd_ref = 0; // Default value
                }

                if (component_data.HasMember("mdl_to_ndc_xform") && component_data["mdl_to_ndc_xform"].IsArray()) {
                    const rapidjson::Value& matrix = component_data["mdl_to_ndc_xform"];

                    if (matrix.Size() == 3) {
                        glm::mat3 xform(0.0f); // Initialize to zero matrix

                        for (rapidjson::SizeType i = 0; i < 3; ++i) {
                            const rapidjson::Value& row = matrix[i];
                            if (row.IsArray() && row.Size() == 3) {
                                for (rapidjson::SizeType j = 0; j < 3; ++j) {
                                    xform[j][i] = row[j].GetFloat(); // Swapped indices due to column-major order
                                }
                            }
                            else {
                                // Handle error: Row is not a valid array of size 3
                                xform = glm::mat3(0.0f); // Set to default zero matrix
                                break;
                            }
                        }

                        graphics_component.mdl_to_ndc_xform = xform;
                    }
                    else {
                        // Handle error: Matrix does not have 3 rows
                        graphics_component.mdl_to_ndc_xform = glm::mat3(0.0f); // Set to default zero matrix
                    }
                }
                else {
                    graphics_component.mdl_to_ndc_xform = glm::mat3(0.0f); // Default zero matrix
                }

                // Add component to entity
                ecs_manager.add_component<Graphics_Component>(entity, graphics_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Graphics_Component to entity ID %u.", entity);
            }
            // ------------------------------------ Collision_Component -------------------------------------------
            else if (component_name == "Collision_Component") {
                // Parse Collision_Component
                Collision_Component collision_component;

                if (component_data.HasMember("width") && component_data["width"].IsNumber()) {
                    collision_component.width = component_data["width"].GetFloat();
                }

                if (component_data.HasMember("height") && component_data["height"].IsNumber()) {
                    collision_component.height = component_data["height"].GetFloat();
                }

                if (component_data.HasMember("collidable") && component_data["collidable"].IsBool()) {
                    collision_component.collidable = component_data["collidable"].GetBool();
                }

                // Add component to entity
                ecs_manager.add_component<Collision_Component>(entity, collision_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Collision_Component to entity ID %u.", entity);
            }
            // ------------------------------------- Audio_Component ---------------------------------------------
            else if (component_name == "Audio_Component") {
                // Parse Audio_Component
                Audio_Component audio_component;

                if (component_data.HasMember("sounds") && component_data["sounds"].IsArray()) {
                    const auto& sounds_array = component_data["sounds"];

                    for (const auto& sound : sounds_array.GetArray()) {
                        if ((sound.HasMember("key") && sound["key"].IsString()) &&
                            (sound.HasMember("filepath") && sound["filepath"].IsString())) {
                            std::string key = sound["key"].GetString();
                            std::string filepath = sound["filepath"].GetString();

                            // Remove .wav extension if present
                            size_t extension = filepath.find(".wav");
                            if (extension != std::string::npos) {
                                filepath = filepath.substr(0, extension);
                            }

                            // Verify audio file exists
                            if (!ASM.load_audio_file(filepath)) {
                                LM.write_log("Warning: Audio file not found for %s", filepath.c_str());
                                continue;
                            }

                            // Get the full path using Assets Manager
                            std::string full_filepath = ASM.get_audio_path(filepath);

                            // Get other properties with defaults
                            PlayState play_state = NONE;
                            if (sound.HasMember("audio_state") && sound["audio_state"].IsInt()) {
                                play_state = static_cast<PlayState>(sound["audio_state"].GetInt());
                            }

                            AudioType audio_type = SFX;
                            if (sound.HasMember("audio_type") && sound["audio_type"].IsInt()) {
                                audio_type = static_cast<AudioType>(sound["audio_type"].GetInt());
                            }

                            float volume = 1.0f;
                            if (sound.HasMember("volume") && sound["volume"].IsFloat()) {
                                volume = sound["volume"].GetFloat();
                            }

                            float pitch = 1.0f;
                            if (sound.HasMember("pitch") && sound["pitch"].IsFloat()) {
                                pitch = sound["pitch"].GetFloat();
                            }

                            bool islooping = false;
                            if (sound.HasMember("islooping") && sound["islooping"].IsBool()) {
                                islooping = sound["islooping"].GetBool();
                            }
                            else if (sound.HasMember("is_looping") && sound["is_looping"].IsBool()) {
                                islooping = sound["is_looping"].GetBool();
                            }

                            // Add sound to component
                            audio_component.add_sound(key, filepath, play_state, audio_type, volume, pitch, islooping);

                            LM.write_log("Added sound - Key: %s, Path: %s, State: %d, Type: %d, Volume: %.2f, Pitch: %.2f, Loop: %d",
                                key.c_str(), filepath.c_str(), play_state, audio_type, volume, pitch, islooping);
                        }
                        else {
                            LM.write_log("Warning: Sound missing required key or filepath properties");
                        }
                    }
                }

                // Handle 3D audio properties
                if (component_data.HasMember("is_3d")) {
                    audio_component.set_is3d(component_data["is_3d"].GetBool());
                }

                if (component_data.HasMember("position") && component_data["position"].IsArray()) {
                    const auto& pos = component_data["position"];
                    Vec3D position(
                        pos[0].GetFloat(),
                        pos[1].GetFloat(),
                        pos[2].GetFloat()
                    );
                    audio_component.set_position(position);
                }

                if (component_data.HasMember("min_distance")) {
                    audio_component.set_min_distance(component_data["min_distance"].GetFloat());
                }

                if (component_data.HasMember("max_distance")) {
                    audio_component.set_max_distance(component_data["max_distance"].GetFloat());
                }

                // Add component to entity
                ecs_manager.add_component<Audio_Component>(entity, audio_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Audio_Component to entity ID %u", entity);

                // Log all sounds in the component for verification
                const auto& sounds = audio_component.get_sounds();
                for (const auto& sound : sounds) {
                    LM.write_log("Verified sound in component - Key: %s, Path: %s",
                        sound.key.c_str(), sound.filepath.c_str());
                }

            }
            // ------------------------------------ GUI_Component -------------------------------------------
            else if (component_name == "GUI_Component") {
                // Parse GUI_Component
                GUI_Component gui_component;

                if (component_data.HasMember("is_container") && component_data["is_container"].IsBool()) {
                    gui_component.is_container = component_data["is_container"].GetBool();
                }

                if (component_data.HasMember("is_progress_bar") && component_data["is_progress_bar"].IsBool()) {
                    gui_component.is_progress_bar = component_data["is_progress_bar"].GetBool();
                }

                if (component_data.HasMember("progress") && component_data["progress"].IsNumber()) {
                    gui_component.progress = component_data["progress"].GetFloat();
                }

                if (component_data.HasMember("is_visible") && component_data["is_visible"].IsBool()) {
                    gui_component.is_visible = component_data["is_visible"].GetBool();
                }

                if (component_data.HasMember("relative_pos") && component_data["relative_pos"].IsArray()) {
                    const rapidjson::Value& pos = component_data["relative_pos"];
                    gui_component.relative_pos.x = pos[0].GetFloat();
                    gui_component.relative_pos.y = pos[1].GetFloat();
                }

                // Add component to entity
                ecs_manager.add_component<GUI_Component>(entity, gui_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added GUI_Component to entity ID %u.", entity);
            }
            // ------------------------------------ Animation_Component -------------------------------------------
            else if (component_name == "Animation_Component") {
                // Parse Animation_Component
                Animation_Component animation_component;

                if (component_data.HasMember("animations") && component_data["animations"].IsArray()) {
                    const rapidjson::Value& map = component_data["animations"];

                    if (map.Size() > 0) {
                        std::map<std::string, std::string> animation_map;
                        for (rapidjson::SizeType i = 0; i < map.Size(); ++i) {
                            const rapidjson::Value& row = map[i];
                            if (row.IsArray() && row.Size() == 2) {
                                if (row[0].IsString() && row[1].IsString()) {
                                    animation_map.insert({ row[0].GetString(), row[1].GetString() });
                                }
                            }
                        }

                        animation_component.animations = animation_map;
                    }
                    else {
                        animation_component.animations = std::map<std::string, std::string>{ {DEFAULT_ANIMATION_IDX, DEFAULT_ANIMATION_NAME} }; // Default value 
                    }
                }
                else {
                    animation_component.animations = std::map<std::string, std::string>{ {DEFAULT_ANIMATION_IDX, DEFAULT_ANIMATION_NAME} }; // Default value
                }

                if (component_data.HasMember("curr_animation_idx") && component_data["curr_animation_idx"].IsUint()) {
                    animation_component.curr_animation_idx = component_data["curr_animation_idx"].GetUint();
                }
                else {
                    animation_component.curr_animation_idx = std::stoi(DEFAULT_ANIMATION_IDX); // Default value
                }

                if (component_data.HasMember("start_animation_idx") && component_data["start_animation_idx"].IsUint()) {
                    animation_component.start_animation_idx = component_data["start_animation_idx"].GetUint();
                }
                else {
                    animation_component.start_animation_idx = std::stoi(DEFAULT_ANIMATION_IDX); // Default value
                }

                if (component_data.HasMember("curr_frame_index") && component_data["curr_frame_index"].IsUint()) {
                    animation_component.curr_frame_index = component_data["curr_frame_index"].GetUint();
                }
                else {
                    animation_component.curr_frame_index = DEFAULT_FRAME_INDEX; // Default value
                }

                if (component_data.HasMember("start_tile_health") && component_data["start_tile_health"].IsUint()) {
                    animation_component.start_tile_health = component_data["start_tile_health"].GetUint();
                }
                else {
                    animation_component.start_tile_health = DEFAULT_TILE_HEALTH; // Default value
                }

                if (component_data.HasMember("curr_tile_health") && component_data["curr_tile_health"].IsUint()) {
                    animation_component.curr_tile_health = component_data["curr_tile_health"].GetUint();
                }
                else {
                    animation_component.curr_tile_health = DEFAULT_TILE_HEALTH; // Default value
                }

                // Add component to entity
                ecs_manager.add_component<Animation_Component>(entity, animation_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Animation_Component to entity ID %u.", entity);
                }
            // ------------------------------------ Logic_Component -------------------------------------------
            else if (component_name == "Logic_Component") {
                // Log the raw value from JSON
                int pattern_value = component_data["movement_pattern"].GetInt();
                LM.write_log("Parsing Logic Component - Raw movement pattern value: %d", pattern_value);

                Logic_Component logic(
                    static_cast<Logic_Component::LogicType>(component_data["logic_type"].GetInt()),
                    static_cast<Logic_Component::MovementPattern>(pattern_value)
                );

                // Set other properties
                if (component_data.HasMember("is_active")) {
                    logic.is_active = component_data["is_active"].GetBool();
                }
                if (component_data.HasMember("movement_speed")) {
                    logic.movement_speed = component_data["movement_speed"].GetFloat();
                }
                if (component_data.HasMember("movement_range")) {
                    logic.movement_range = component_data["movement_range"].GetFloat();
                }
                if (component_data.HasMember("reverse_direction")) {
                    logic.reverse_direction = component_data["reverse_direction"].GetBool();
                }
                if (component_data.HasMember("rotate_with_motion")) {
                    logic.rotate_with_motion = component_data["rotate_with_motion"].GetBool();
                }
                if (component_data.HasMember("origin_pos") && component_data["origin_pos"].IsArray()) {
                    logic.origin_pos.x = component_data["origin_pos"][0].GetFloat();
                    logic.origin_pos.y = component_data["origin_pos"][1].GetFloat();
                }

                // Log the final state
                LM.write_log("Created Logic Component with movement pattern: %d, speed: %.2f, range: %.2f",
                    static_cast<int>(logic.movement_pattern),
                    logic.movement_speed,
                    logic.movement_range);

                // Add component to entity
                ecs_manager.add_component<Logic_Component>(entity, logic);
                LM.write_log("Component_Parser::add_components_from_json(): Added Logic_Component to entity ID %u.", entity);
                }
            // ------------------------------------ Text_Component -------------------------------------------
            else if (component_name == "Text_Component") {
                // Parse Text_Component
                Text_Component text_component;

                if (component_data.HasMember("font_name") && component_data["font_name"].IsString()) {
                    text_component.font_name = component_data["font_name"].GetString();
                }

                if (component_data.HasMember("text") && component_data["text"].IsString()) {
                    text_component.text = component_data["text"].GetString();
                }

                if (component_data.HasMember("color") && component_data["color"].IsArray()) {
                    const rapidjson::Value& clr = component_data["color"];
                    text_component.color.x = clr[0].GetFloat();
                    text_component.color.y = clr[1].GetFloat();
                    text_component.color.z = clr[2].GetFloat();
                }

                // Add component to entity
                ecs_manager.add_component<Text_Component>(entity, text_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Text_Component to entity ID %u.", entity);
                }
            // ------------------------------------ Unknown Component -------------------------------------------
            else {
                LM.write_log("Component_Parser::add_components_from_json(): Unknown component '%s' for entity ID %u. Skipping.", component_name.c_str(), entity);
            }
        }
    }
} // namespace lof