/**
 * @file ECS_Manager.cpp
 * @brief Implements the ECS_Manager class helper functions.
 * @author Simon Chan (97.56%), Liliana Hanawardani (2.439%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

 // Include header file
#include "ECS_Manager.h"

// Include for components
#include "../Component/Component.h"

// Include System.h and systems
#include "../System/System.h"
#include "../System/Movement_System.h"
#include "../System/Render_System.h" 
#include "../System/Collision_System.h"

// Include Entity.h
#include "../Entity/Entity.h"

// Include Utility headers
#include "../Utility/Type.h" // Include shared types
#include "../Utility/Component_Parser.h" // Include Component_Parser for adding components from JSON

// Include Log_Manager for logging
#include "Log_Manager.h"

// Include standard headers
#include <algorithm>
#include <cassert>
#include <chrono>

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
            LM.write_log("ECS_Manager::start_up(): ECS_Manager is already started.");
            return 0; // Already started
        }

        try {
            // Register all components used in the game
            LM.write_log("ECS_Manager::start_up(): Registering components.");

            register_component<Transform2D>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Transform2D'.");

            register_component<Velocity_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Velocity_Component'.");

            register_component<Physics_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Physics_Component'.");

            register_component<Graphics_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Graphics_Component'.");

            register_component<Collision_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Collision_Component'.");

            // Register all systems used in the game
            LM.write_log("ECS_Manager::start_up(): Adding systems.");

            add_system(std::make_unique<Movement_System>());
            LM.write_log("ECS_Manager::start_up(): Added system 'Movement_System'.");

            add_system(std::make_unique<Render_System>());
            LM.write_log("ECS_Manager::start_up(): Added system 'Render_System'.");

            add_system(std::make_unique<Collision_System>());
            LM.write_log("ECS_Manager::start_up(): Added system 'Collision_System'.");

            m_is_started = true;
            LM.write_log("ECS_Manager::start_up(): ECS_Manager started successfully.");
            return 0;
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager::start_up(): Failed to initialize. Error: %s", e.what());
            return -1;
        }
    }

    void ECS_Manager::shut_down() {
        if (!is_started()) {
            LM.write_log("ECS_Manager::shut_down(): ECS_Manager is not started. Nothing to shut down.");
            return; // Not started, nothing to shut down
        }

        // Perform shutdown tasks specific to ECS_Manager
        systems.clear();
        LM.write_log("ECS_Manager::shut_down(): Cleared all systems.");

        entities.clear();
        LM.write_log("ECS_Manager::shut_down(): Cleared all entities.");

        component_arrays.clear();
        LM.write_log("ECS_Manager::shut_down(): Cleared all component arrays.");

        component_type_to_id.clear();
        LM.write_log("ECS_Manager::shut_down(): Cleared component type to ID mappings.");

        id_to_component_type.clear();
        LM.write_log("ECS_Manager::shut_down(): Cleared ID to component type mappings.");

        next_component_id = 0;
        LM.write_log("ECS_Manager::shut_down(): Reset next component ID to 0.");

        m_is_started = false; // Indicate that the manager is no longer started
        LM.write_log("ECS_Manager::shut_down(): ECS_Manager shut down successfully.");
    }

    void ECS_Manager::add_components_from_json(EntityID entity, const rapidjson::Value& components) {
        LM.write_log("ECS_Manager::add_components_from_json(): Adding components to entity ID %u.", entity);
        Component_Parser::add_components_from_json(*this, entity, components);
    }

    EntityID ECS_Manager::clone_entity_from_prefab(const std::string& prefab_name) {
        LM.write_log("ECS_Manager::clone_entity_from_prefab(): Cloning entity from prefab '%s'.", prefab_name.c_str());
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

        LM.write_log("ECS_Manager::clone_entity_from_prefab(): Entity ID %u cloned from prefab '%s' successfully.", eid, prefab_name.c_str());
        return eid;
    }

    EntityID ECS_Manager::create_entity() {
        EntityID id = static_cast<EntityID>(entities.size());
        entities.emplace_back(std::make_unique<Entity>(id));
        LM.write_log("ECS_Manager::create_entity(): Created entity with ID %u.", id);

        // Update systems with the new entity
        update_entity_in_systems(id);

        return id;
    }

    void ECS_Manager::destroy_entity(EntityID entity) {
        // Remove entity from systems
        for (auto& system : systems) {
            system->remove_entity(entity);
        }

        // Remove components associated with the entity
        for (auto& pair : component_arrays) {
            const std::type_index& typeIndex = pair.first;
            auto& componentArray = pair.second;
            if (entity < componentArray.size()) {
                componentArray[entity].reset();
            }
        }

        LM.write_log("ECS_Manager::destroy_entity(): Destroyed entity with ID %u.", entity);
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
        //LM.write_log("ECS_Manager::get_entities(): Retrieving list of entities.");
        return entities;
    }

    void ECS_Manager::update_entity_in_systems(EntityID entity) {
        const ComponentMask& entity_mask = entities[entity]->get_component_mask();

        for (auto& system : systems) {
            const Signature& system_signature = system->get_signature();

            // Check if entity_mask includes all bits set in system_signature
            bool matches = (entity_mask & system_signature) == system_signature;

            if (matches) {
                system->add_entity(entity);
                LM.write_log("Entity %u added to system %s.", entity, system->get_type().c_str());
            }
            else {
                system->remove_entity(entity);
                LM.write_log("Entity %u removed from system %s.", entity, system->get_type().c_str());
            }
        }
    }

} // namespace lof
