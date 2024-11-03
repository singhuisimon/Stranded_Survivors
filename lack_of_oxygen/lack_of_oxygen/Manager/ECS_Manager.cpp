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
#include "../System/GUI_System.h"
#include "../System/Audio_System.h"

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

            register_component<Collision_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Collision_Component'.");

            register_component<Physics_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Physics_Component'.");

            register_component<Graphics_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Graphics_Component'.");

            register_component<Audio_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'Audio_Component'.");

            register_component<GUI_Component>();
            LM.write_log("ECS_Manager::start_up(): Registered component 'GUI_Component'.");

            // Register all systems used in the game
            LM.write_log("ECS_Manager::start_up(): Adding systems.");

            add_system(std::make_unique<Collision_System>());
            LM.write_log("ECS_Manager::start_up(): Added system 'Collision_System'.");

            add_system(std::make_unique<Movement_System>());
            LM.write_log("ECS_Manager::start_up(): Added system 'Movement_System'.");

            add_system(std::make_unique<Render_System>());
            LM.write_log("ECS_Manager::start_up(): Added system 'Render_System'.");

            add_system(std::make_unique<GUI_System>(*this));
            LM.write_log("ECS_Manager::start_up(): Added system 'GUI_System'.");

            add_system(std::make_unique<Audio_System>());
            LM.write_log("ECS_Manager::start_up(): Added system 'Audio_System'.");

            m_is_started = true;
            LM.write_log("ECS_Manager::start_up(): ECS_Manager started successfully.");
            return 0;
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager::start_up(): Failed to initialize. Error: %s", e.what());
            return -1;
        }
    }


    // Update shut_down to clear the name map
    void ECS_Manager::shut_down() {
        if (!is_started()) {
            LM.write_log("ECS_Manager::shut_down(): ECS_Manager is not started. Nothing to shut down.");
            return;
        }

        systems.clear();
        entities.clear();
        entity_names.clear(); // Clear the name map
        component_arrays.clear();
        component_type_to_id.clear();
        id_to_component_type.clear();
        next_component_id = 0;

        m_is_started = false;
        LM.write_log("ECS_Manager::shut_down(): ECS_Manager shut down successfully.");
    }


    void ECS_Manager::add_components_from_json(EntityID entity, const rapidjson::Value& components) {
        LM.write_log("ECS_Manager::add_components_from_json(): Adding components to entity ID %u.", entity);
        Component_Parser::add_components_from_json(*this, entity, components);
    }


    EntityID ECS_Manager::clone_entity_from_prefab(const std::string& prefab_name, const std::string& entity_name) {
        LM.write_log("ECS_Manager::clone_entity_from_prefab(): Cloning entity from prefab '%s'.", prefab_name.c_str());

        // Validate prefab exists
        const rapidjson::Value* prefab = SM.get_prefab(prefab_name);
        if (!prefab) {
            LM.write_log("ECS_Manager::clone_entity_from_prefab(): Prefab '%s' not found.", prefab_name.c_str());
            return INVALID_ENTITY_ID;
        }

        // Validate prefab has components
        if (!prefab->HasMember("components") || !(*prefab)["components"].IsObject()) {
            LM.write_log("ECS_Manager::clone_entity_from_prefab(): Prefab '%s' does not have valid components object.", prefab_name.c_str());
            return INVALID_ENTITY_ID;
        }

        // Determine the entity name
        std::string final_name;
        if (!entity_name.empty()) {
            // Use provided name
            final_name = entity_name;
        }
        else {
            // Generate a name based on prefab name and unique identifier
            final_name = prefab_name + "_" + std::to_string(entities.size());
        }

        // Ensure name uniqueness
        std::string unique_name = final_name;
        int suffix = 1;
        while (entity_names.find(unique_name) != entity_names.end()) {
            unique_name = final_name + "_" + std::to_string(suffix++);
        }

        // Create new entity with the unique name
        EntityID eid = create_entity(unique_name);
        if (eid == INVALID_ENTITY_ID) {
            LM.write_log("ECS_Manager::clone_entity_from_prefab(): Failed to create entity.");
            return INVALID_ENTITY_ID;
        }

        try {
            // Add components from prefab
            const rapidjson::Value& components = (*prefab)["components"];
            add_components_from_json(eid, components);

            LM.write_log("ECS_Manager::clone_entity_from_prefab(): Successfully cloned entity '%s' with ID %u from prefab '%s'",
                unique_name.c_str(), eid, prefab_name.c_str());
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager::clone_entity_from_prefab(): Error adding components to entity: %s", e.what());
            destroy_entity(eid); // Clean up the partially created entity
            return INVALID_ENTITY_ID;
        }

        return eid;
    }


    EntityID ECS_Manager::create_entity(const std::string& name) {
        EntityID id = static_cast<EntityID>(entities.size());
        entities.emplace_back(std::make_unique<Entity>(id, name));

        // If name is provided, store it in the lookup map
        if (!name.empty()) {
            // Check if name already exists
            if (entity_names.find(name) != entity_names.end()) {
                LM.write_log("ECS_Manager::create_entity(): Warning: Entity name '%s' already exists. Using name with ID suffix.", name.c_str());
                // If name already exists, append ID to make it unique
                std::string unique_name = name + "_" + std::to_string(id);
                entity_names[unique_name] = id;
                // Update the entity's name to match
                entities.back()->set_name(unique_name);
            }
            else {
                entity_names[name] = id;
            }
            LM.write_log("ECS_Manager::create_entity(): Created entity '%s' with ID %u.", entities.back()->get_name().c_str(), id);
        }
        else {
            LM.write_log("ECS_Manager::create_entity(): Created unnamed entity with ID %u.", id);
        }

        // Resize component arrays if necessary
        for (auto& component_pair : component_arrays) {
            if (component_pair.second.size() <= id) {
                component_pair.second.resize(entities.size());
            }
        }

        // Update systems with the new entity
        update_entity_in_systems(id);
        return id;
    }


    void ECS_Manager::destroy_entity(EntityID entity) {
        if (entity >= entities.size() || !entities[entity]) {
            LM.write_log("ECS_Manager::destroy_entity(): Invalid entity ID or already destroyed: %u", entity);
            return;
        }

        // Remove from systems
        for (auto& system : systems) {
            system->remove_entity(entity);
        }

        // Remove from name lookup if it has a name
        const std::string& name = entities[entity]->get_name();
        if (!name.empty()) {
            entity_names.erase(name);
            LM.write_log("ECS_Manager::destroy_entity(): Removed name mapping for '%s'", name.c_str());
        }

        // Remove components
        for (auto& component_pair : component_arrays) {
            auto& componentArray = component_pair.second;
            if (entity < componentArray.size()) {
                componentArray[entity].reset();
            }
        }

        // Clear the entity itself
        entities[entity].reset();

        LM.write_log("ECS_Manager::destroy_entity(): Destroyed entity ID %u.", entity);
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

    Entity* ECS_Manager::get_entity(EntityID entity_id) {
        if (entity_id < entities.size()) {
            return entities[entity_id].get();
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Entity>>& ECS_Manager::get_entities() const {
        return entities;
    }

    EntityID ECS_Manager::find_entity_by_name(const std::string& name) const {
        auto it = entity_names.find(name);
        if (it != entity_names.end()) {
            return it->second;
        }
        return INVALID_ENTITY_ID;
    }

    bool ECS_Manager::update_entity_name(EntityID entity_id, const std::string& new_name) {
        if (auto* entity = get_entity(entity_id)) {
            // First check if new name already exists (unless it's empty)
            if (!new_name.empty() && entity_names.find(new_name) != entity_names.end()) {
                LM.write_log("ECS_Manager::update_entity_name(): Name '%s' already exists.", new_name.c_str());
                return false;
            }

            // Remove old name from map if it exists
            const std::string& old_name = entity->get_name();
            if (!old_name.empty()) {
                entity_names.erase(old_name);
                LM.write_log("ECS_Manager::update_entity_name(): Removed old name mapping for '%s'", old_name.c_str());
            }

            // Add new name if provided
            if (!new_name.empty()) {
                entity_names[new_name] = entity_id;
                LM.write_log("ECS_Manager::update_entity_name(): Added new name mapping for '%s'", new_name.c_str());
            }

            entity->set_name(new_name);
            LM.write_log("ECS_Manager::update_entity_name(): Updated entity %u name from '%s' to '%s'",
                entity_id, old_name.c_str(), new_name.c_str());
            return true;
        }

        LM.write_log("ECS_Manager::update_entity_name(): Entity %u not found", entity_id);
        return false;
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
