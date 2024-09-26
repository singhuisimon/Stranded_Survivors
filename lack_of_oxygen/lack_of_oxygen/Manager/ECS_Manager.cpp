/**
 * @file ECS_Manager.cpp
 * @brief Implements the ECS_Manager class methods.
 * @date September 21, 2024
 */

#include "ECS_Manager.h"

 // Include Component.h since all components are defined there
#include "../Component/Component.h"

// Include System.h and specific systems as needed
#include "../System/System.h"
#include "../System/Movement_System.h" // Include other systems as needed
#include "../System/Render_System.h" 

// Include Entity.h
#include "../Entity/Entity.h"

// Include Log_Manager for logging
#include "Log_Manager.h"

// Include standard headers
#include <algorithm>
#include <cassert>

namespace lof {

    // Initialize static members
    ECS_Manager& ECS_Manager::get_instance() {
        static ECS_Manager instance;
        return instance;
    }

    ECS_Manager::ECS_Manager() {
        set_type("ECS_Manager");
        m_is_started = false;
    }

    int ECS_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        try {
            LM.write_log("ECS_Manager::start_up(): Registering components.");

            // Register components
            register_component<Transform2D>();
            register_component<Velocity_Component>();
            register_component<Model_Component>();
            register_component<Mass_Component>();
            register_component<Model_Component>(); 
            register_component<Graphics_Component>(); 

            LM.write_log("ECS_Manager::start_up(): Adding systems.");

            // Add systems
            add_system(std::make_unique<Movement_System>(*this)); 
            
            // Add other systems as needed
            add_system(std::make_unique<Render_System>(*this)); 

            m_is_started = true;
            LM.write_log("ECS_Manager::start_up(): Started successfully.");
            return 0;
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager::start_up(): Failed to initialize. Error: %s", e.what());
            return -1;
        }
    }

    void ECS_Manager::shut_down() {
        if (!is_started()) {
            return; // Not started, nothing to shut down
        }

        // Perform shutdown tasks specific to ECS_Manager
        systems.clear();
        entities.clear();
        component_arrays.clear();
        component_type_to_id.clear();
        id_to_component_type.clear();
        next_component_id = 0;

        m_is_started = false; // Indicate that the manager is no longer started
        LM.write_log("ECS_Manager::shut_down(): ECS_Manager shut down successfully.");
    }

    EntityID ECS_Manager::create_entity() {
        EntityID id = static_cast<EntityID>(entities.size());
        entities.emplace_back(std::make_unique<Entity>(id));
        LM.write_log("ECS_Manager::create_entity(): Created entity with ID %u.", id);
        return id;
    }

    void ECS_Manager::add_system(std::unique_ptr<System> system) {
        // Get the system type before moving
        std::string system_type = system->get_type();
        LM.write_log("ECS_Manager::add_system(): Adding system '%s'.", system_type.c_str());

        systems.emplace_back(std::move(system));

        LM.write_log("ECS_Manager::add_system(): System '%s' added successfully.", systems.back()->get_type().c_str());
    }

    void ECS_Manager::update(float delta_time) {
        for (auto& system : systems) {
            system->update(delta_time);
        }
    }

    const std::vector<std::unique_ptr<Entity>>& ECS_Manager::get_entities() const {
        return entities;
    }

    int ECS_Manager::load_entities(const std::vector<EntityConfig>& entities_config,
        const std::unordered_map<std::string, std::shared_ptr<Model>>& models) {
        for (const auto& entity_config : entities_config) {
            // Create entity
            EntityID new_entity = create_entity();
            LM.write_log("ECS_Manager::load_entities(): Created entity with ID: %u", new_entity);

            // Add Graphics_Component manually for each entity (FOR TESTING) 
            GLuint mdl_ref = 0; 
            GLuint shd_ref = 0; 
            glm::mat3 mdl_to_ndc_xform = glm::mat3{ 0 }; 
            Graphics_Component graphics_component(mdl_ref, shd_ref, mdl_to_ndc_xform);
            add_component<Graphics_Component>(new_entity, graphics_component); 
            LM.write_log("ECS_Manager::load_entities(): Added Graphics_Component MANUALLY to entity ID %u.", new_entity);

            // Add components to the entity
            for (const auto& comp_config : entity_config.components) {
                if (comp_config.type == "Transform2D") {
                    // Parse properties
                    Vec2D position(0.0f, 0.0f);
                    float rotation = 0.0f;
                    Vec2D scale(1.0f, 1.0f);

                    const auto& properties = comp_config.properties;

                    // Parse position
                    if (properties.HasMember("position") && properties["position"].IsArray() && properties["position"].Size() == 2) {
                        position.x = properties["position"][0].GetFloat();
                        position.y = properties["position"][1].GetFloat();
                    }

                    // Parse rotation
                    if (properties.HasMember("rotation") && properties["rotation"].IsNumber()) {
                        rotation = properties["rotation"].GetFloat();
                    }

                    // Parse scale
                    if (properties.HasMember("scale") && properties["scale"].IsArray() && properties["scale"].Size() == 2) {
                        scale.x = properties["scale"][0].GetFloat();
                        scale.y = properties["scale"][1].GetFloat();
                    }

                    // Add Transform2D component
                    Transform2D transform_component(position, rotation, scale);
                    add_component<Transform2D>(new_entity, transform_component);
                    LM.write_log("ECS_Manager::load_entities(): Added Transform2D component to entity ID %u.", new_entity);
                }
                else if (comp_config.type == "Velocity_Component") {
                    // Parse properties
                    float vx = 0.0f;
                    float vy = 0.0f;

                    const auto& properties = comp_config.properties;

                    // Parse velocity
                    if (properties.HasMember("velocity") && properties["velocity"].IsArray() && properties["velocity"].Size() == 2) {
                        vx = properties["velocity"][0].GetFloat();
                        vy = properties["velocity"][1].GetFloat();
                    }

                    // Add Velocity_Component
                    Velocity_Component velocity_component(vx, vy);
                    add_component<Velocity_Component>(new_entity, velocity_component);
                    LM.write_log("ECS_Manager::load_entities(): Added Velocity_Component to entity ID %u.", new_entity);
                }
                else if (comp_config.type == "Model_Component") {
                    // Parse properties
                    std::string model_name = "default_model";

                    const auto& properties = comp_config.properties;

                    // Parse model_name
                    if (properties.HasMember("model_name") && properties["model_name"].IsString()) {
                        model_name = properties["model_name"].GetString();
                    }

                    // Retrieve the loaded model from models map
                    std::shared_ptr<Model> model = nullptr;
                    auto it = models.find(model_name);
                    if (it != models.end()) {
                        model = it->second;
                    }
                    else {
                        LM.write_log("ECS_Manager::load_entities(): Model '%s' not found for entity ID %u.", model_name.c_str(), new_entity);
                    }

                    // Add Model_Component with model_ptr
                    Model_Component model_component(model_name, model);
                    add_component<Model_Component>(new_entity, model_component);
                    LM.write_log("ECS_Manager::load_entities(): Added Model_Component to entity ID %u with model '%s'.", new_entity, model_name.c_str());
                }
                //else if (comp_config.type == "Graphics_Component") { // FOR TESTING (ADDING GRAPHICS COMPONENT. Now I'll add manually to all at the top but in the future it will be read from the file)
                //    // Parse properties    
                //    GLuint mdl_ref = 0;
                //    GLuint shd_ref = 0;
                //    glm::mat3 mdl_to_ndc_xform = glm::mat3{ 0 };

                //    const auto& properties = comp_config.properties;


                //    // Parse model_ref
                //    // Parse shader_ref
                //    // Parse transformation matrix
                //    

                //    // Add Graphics_Component
                //    Graphics_Component graphics_component(mdl_ref, shd_ref, mdl_to_ndc_xform); 
                //    add_component<Graphics_Component>(new_entity, graphics_component);
                //    LM.write_log("ECS_Manager::load_entities(): Added Graphics_Component to entity ID %u.", new_entity);
                //}
   
                else {
                    LM.write_log("ECS_Manager::load_entities(): Unknown component type '%s' for entity ID %u.",
                        comp_config.type.c_str(), new_entity);
                }
            }
        }
        return 0;
    }

} // namespace lof
