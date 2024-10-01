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
#include "../System/Collision_System.h"

// Include Entity.h
#include "../Entity/Entity.h"

// Include Log_Manager for logging
#include "Log_Manager.h"

// Include Component_Parser for adding components from JSON
#include "../Utility/Component_Parser.h"

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
        set_time(0);
    }

    int ECS_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        try {
            LM.write_log("ECS_Manager::start_up(): Registering components.");

            // Register all components used in the game
            register_component<Transform2D>();
            register_component<Velocity_Component>();
            //register_component<Model_Component>();
            register_component<Collision_Component>();
            register_component<Mesh_Component>();
            //register_component<Mass_Component>();
            register_component<Physics_Component>();
            register_component<Graphics_Component>(); 

            LM.write_log("ECS_Manager::start_up(): Adding systems.");

            // Add systems
            add_system(std::make_unique<Movement_System>(*this)); 
            //Add collision system
            add_system(std::make_unique<Collision_System>(*this));
            
            // Add other systems as needed
            // Add all systems
            add_system(std::make_unique<Movement_System>(*this));
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

    void ECS_Manager::add_components_from_json(EntityID entity, const rapidjson::Value& components) {
        Component_Parser::add_components_from_json(*this, entity, components);
    }

    EntityID ECS_Manager::clone_entity_from_prefab(const std::string& prefab_name) {
        const rapidjson::Value* prefab = SM.get_prefab(prefab_name);
        if (!prefab) {
            LM.write_log("ECS_Manager::clone_entity_from_prefab(): Prefab '%s' not found.", prefab_name.c_str());
            return INVALID_ENTITY_ID; // Ensure INVALID_ENTITY_ID is defined appropriately
        }
        if (!prefab->HasMember("components") || !(*prefab)["components"].IsObject()) {
            LM.write_log("ECS_Manager::clone_entity_from_prefab(): Prefab '%s' does not have 'components' object.", prefab_name.c_str());
            return INVALID_ENTITY_ID;
        }

        EntityID eid = create_entity();
        LM.write_log("ECS_Manager::clone_entity_from_prefab(): Created entity from prefab '%s' with ID %u.", prefab_name.c_str(), eid);

        const rapidjson::Value& components = (*prefab)["components"];
        add_components_from_json(eid, components);

        return eid;
    }

    EntityID ECS_Manager::create_entity() {
        EntityID id = static_cast<EntityID>(entities.size());
        entities.emplace_back(std::make_unique<Entity>(id));
        LM.write_log("ECS_Manager::create_entity(): Created entity with ID %u.", id);
        return id;
    }

    const std::vector<std::unique_ptr<System>>& ECS_Manager::get_systems() const{
        return systems;
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

            // Getting delta time for each system
            system->set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
            // Updating each system
            system->update(delta_time);
            system->set_time(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - system->get_time());
            
        }
    }

    const std::vector<std::unique_ptr<Entity>>& ECS_Manager::get_entities() const {
        return entities;
    }

} // namespace lof
