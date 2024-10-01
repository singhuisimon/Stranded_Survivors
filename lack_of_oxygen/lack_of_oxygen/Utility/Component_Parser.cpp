/**
 * @file ComponentParser.cpp
 * @brief Implements the ComponentParser utility class methods.
 * @date October 1, 2024
 */

 // Include base file
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

            if (component_name == "Transform2D") {
                // Parse Transform2D component
                Transform2D transform;
                if (component_data.HasMember("position") && component_data["position"].IsArray()) {
                    const rapidjson::Value& pos = component_data["position"];
                    transform.position.x = pos[0].GetFloat();
                    transform.position.y = pos[1].GetFloat();
                }
                if (component_data.HasMember("rotation") && component_data["rotation"].IsNumber()) {
                    transform.rotation = component_data["rotation"].GetFloat();
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
            else if (component_name == "Mesh_Component") {
                // Parse Mesh_Component
                Mesh_Component mesh;
                if (component_data.HasMember("mesh_name") && component_data["mesh_name"].IsString()) {
                    mesh.mesh_name = component_data["mesh_name"].GetString();
                }

                // Add component to entity
                ecs_manager.add_component<Mesh_Component>(entity, mesh);
                LM.write_log("Component_Parser::add_components_from_json(): Added Mesh_Component to entity ID %u.", entity);
            }
            //else if (component_name == "Mass_Component") {
            //    // Parse Mass_Component
            //    Mass_Component mass_component;
            //    if (component_data.HasMember("mass") && component_data["mass"].IsNumber()) {
            //        mass_component.set_mass(component_data["mass"].GetFloat());
            //    }
            //    if (component_data.HasMember("is_static") && component_data["is_static"].IsBool()) {
            //        mass_component.is_static = component_data["is_static"].GetBool();
            //    }

                // Add component to entity
                ecs_manager.add_component<Mass_Component>(entity, mass_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Mass_Component to entity ID %u.", entity);
            }
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



                // Add component to entity
                ecs_manager.add_component<Physics_Component>(entity, physics_component);
                LM.write_log("Component_Parser::add_components_from_json(): Added Physics_Component to entity ID %u.", entity);
            }
            else if (component_name == "Graphics_Component") {
                // Parse Graphics_Component
                Graphics_Component graphics_component;

                if (component_data.HasMember("mdl_ref") && component_data["mdl_ref"].IsUint()) {
                    graphics_component.mdl_ref = component_data["mdl_ref"].GetUint();
                }
                else {
                    graphics_component.mdl_ref = 0; // Default value
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
            else {
                LM.write_log("Component_Parser::add_components_from_json(): Unknown component '%s' for entity ID %u. Skipping.", component_name.c_str(), entity);
            }
        }
    }
} // namespace lof
