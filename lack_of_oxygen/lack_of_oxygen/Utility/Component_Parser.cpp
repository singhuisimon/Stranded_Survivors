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
                    physics_component.gravity.x = grav[0].GetFloat();
                    physics_component.gravity.y = grav[1].GetFloat();
                }

                if (component_data.HasMember("damping_factor") && component_data["damping_factor"].IsNumber()) {
                    physics_component.damping_factor = component_data["damping_factor"].GetFloat();
                }

                if (component_data.HasMember("max_velocity") && component_data["max_velocity"].IsNumber()) {
                    physics_component.max_velocity = component_data["max_velocity"].GetFloat();
                }

                if (component_data.HasMember("accumulated_force") && component_data["accumulated_force"].IsArray()) {
                    const rapidjson::Value& acc_force = component_data["accumulated_force"];
                    physics_component.accumulated_force.x = acc_force[0].GetFloat();
                    physics_component.accumulated_force.y = acc_force[1].GetFloat();
                }

                if (component_data.HasMember("mass") && component_data["mass"].IsNumber()) {
                    physics_component.set_mass(component_data["mass"].GetFloat());
                }

                if (component_data.HasMember("is_static") && component_data["is_static"].IsBool()) {
                    physics_component.is_static = component_data["is_static"].GetBool();
                }

                //if (component_data.hasmember("is_moveable") && component_data["is_moveable"].isbool()) {
                //    physics_component.is_moveable = component_data["is_moveable"].getbool();
                //}

                if (component_data.HasMember("is_grounded") && component_data["is_grounded"].IsBool()) {
                    physics_component.is_grounded = component_data["is_grounded"].GetBool();
                }

                //if (component_data.hasmember("is_jumping") && component_data["is_jumping"].isbool()) {
                //    physics_component.is_jumping = component_data["is_jumping"].getbool();
                //}

                if (component_data.HasMember("jump_force") && component_data["jump_force"].IsNumber()) {
                    physics_component.jump_force = component_data["jump_force"].GetFloat();
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

                // Add component to entity
                ecs_manager.add_component<Collision_Component>(entity, collision_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Collision_Component to entity ID %u.", entity);
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
            // ------------------------------------ Unknown Component -------------------------------------------
            else {
                LM.write_log("Component_Parser::add_components_from_json(): Unknown component '%s' for entity ID %u. Skipping.", component_name.c_str(), entity);
            }
        }
    }
} // namespace lof