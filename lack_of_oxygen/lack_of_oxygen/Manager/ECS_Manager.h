/**
 * @file ECS_Manager.h
 * @brief Declares the ECS_Manager class for managing the ECS.
 * @author Simon Chan (100%)
 * @date September 21, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_ECS_MANAGER_H
#define LOF_ECS_MANAGER_H

 // Macros for accessing manager singleton instances
#define ECSM lof::ECS_Manager::get_instance()

// Include header file
#include "Manager.h"

// Include other necessary headers
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../System/System.h"
#include "../Manager/Serialization_Manager.h"

// Include standard headers
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <unordered_set>
#include <cassert> // For assert

namespace lof {

    /**
     * @class ECS_Manager
     * @brief Manages entities, components, and systems in the ECS architecture.
     */
    class ECS_Manager : public Manager {
    private:
        // Private constructor for singleton pattern
        ECS_Manager();

        // Entity storage
        std::vector<std::unique_ptr<Entity>> entities;

        // System storage
        std::vector<std::unique_ptr<System>> systems;

        // Component storage
        std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> component_arrays;
        // Component identification
        std::unordered_map<std::type_index, std::size_t> component_type_to_id;
        std::unordered_map<std::size_t, const std::type_info*> id_to_component_type;
        std::size_t next_component_id = 0;

        // Entity name lookup
        std::unordered_map<std::string, EntityID> entity_names;

        // Helper member functions
        template<typename T>
        std::vector<std::unique_ptr<Component>>& get_component_array();

        /**
         * @brief Update systems' entity lists based on an entity's signature.
         * @param entity The ID of the entity to update.
         */
        void update_entity_in_systems(EntityID entity);

    public:
        /**
         * @brief Get the singleton instance of ECS_Manager.
         * @return Reference to the singleton instance.
         */
        static ECS_Manager& get_instance();

        // Delete copy constructor and assignment operator
        ECS_Manager(const ECS_Manager&) = delete;
        ECS_Manager& operator=(const ECS_Manager&) = delete;

        // Start up and shut down methods
        int start_up() override;
        void shut_down() override;

        // Entity destruction
        void destroy_entity(EntityID entity);

        /**
         * @brief Create an entity with an optional name.
         * @param name Optional name for the entity.
         * @return The ID of the created entity.
         */
        EntityID create_entity(const std::string& name = "");

        /**
         * @brief Get a pointer to an entity by its ID.
         * @param entity_id The ID of the entity to retrieve.
         * @return Pointer to the entity if found, nullptr otherwise.
         */
        Entity* get_entity(EntityID entity_id);

        /**
         * @brief Find an entity by its name.
         * @param name The name of the entity to find.
         * @return The entity ID if found, INVALID_ENTITY_ID otherwise.
         */
        EntityID find_entity_by_name(const std::string& name) const;

        /**
         * @brief Update the name of an existing entity.
         * @param entity_id The ID of the entity to update.
         * @param new_name The new name for the entity.
         * @return True if name was successfully updated, false if entity not found or name already exists.
         */
        bool update_entity_name(EntityID entity_id, const std::string& new_name);

        /**
         * @brief Clone an entity based on a prefab.
         * @param prefab_name The name of the prefab to clone from.
         * @param entity_name Optional name for the new entity.
         * @return The ID of the newly created entity, or INVALID_ENTITY_ID if failed.
         */
        EntityID clone_entity_from_prefab(const std::string& prefab_name, const std::string& entity_name = "");

        /**
         * @brief Add components to an entity from JSON data.
         * @param entity The entity ID.
         * @param components The JSON value containing component data.
         */
        void add_components_from_json(EntityID entity, const rapidjson::Value& components);

        // Component template functions
        template<typename T>
        void register_component();

        template<typename T>
        void add_component(EntityID entity, T component);

        template<typename T>
        void remove_component(EntityID entity);

        template<typename T>
        T& get_component(EntityID entity);

        template<typename T>
        bool has_component(EntityID entity) const;

        template<typename T>
        std::size_t get_component_id() const;

        // System management
        void add_system(std::unique_ptr<System> system);
        void update(float delta_time);

        // Accessing each system
        const std::vector<std::unique_ptr<System>>& get_systems() const;

        // Access entities
        const std::vector<std::unique_ptr<Entity>>& get_entities() const;
    };

    // Template Definitions
    // **All template definitions are within the `lof` namespace**

    template<typename T>
    void ECS_Manager::register_component() {
        std::type_index typeIndex(typeid(T));

        // Ensure component type hasn't been registered before
        assert(component_type_to_id.find(typeIndex) == component_type_to_id.end() && "Component type already registered.");

        component_type_to_id[typeIndex] = next_component_id;
        id_to_component_type[next_component_id] = &typeid(T);
        component_arrays[typeIndex] = std::vector<std::unique_ptr<Component>>(entities.size());

        next_component_id++;

        // Ensure next_component_id doesn't exceed MAX_COMPONENTS
        // Assuming MAX_COMPONENTS is defined elsewhere
        assert(next_component_id <= MAX_COMPONENTS && "Exceeded maximum number of components.");
    }

    template<typename T>
    void ECS_Manager::add_component(EntityID entity, T component) {
        std::type_index typeIndex(typeid(T));

        // Ensure component type is registered
        assert(component_type_to_id.find(typeIndex) != component_type_to_id.end() && "Component type not registered.");

        // Ensure entity ID is valid
        assert(entity < entities.size() && "Entity ID out of range.");

        // Add component to entity
        entities[entity]->add_component(component_type_to_id[typeIndex]);

        // Store component
        auto& componentArray = get_component_array<T>();

        // Resize the component array if necessary
        if (componentArray.size() <= entity) {
            componentArray.resize(entities.size());
        }

        componentArray[entity] = std::make_unique<T>(component);

        // Update systems
        update_entity_in_systems(entity);
    }

    template<typename T>
    void ECS_Manager::remove_component(EntityID entity) {
        std::type_index typeIndex(typeid(T));

        // Ensure component type is registered
        assert(component_type_to_id.find(typeIndex) != component_type_to_id.end() && "Component type not registered.");

        // Ensure entity ID is valid
        assert(entity < entities.size() && "Entity ID out of range.");

        // Remove component from entity
        entities[entity]->remove_component(component_type_to_id[typeIndex]);

        // Remove component from storage
        auto& componentArray = get_component_array<T>();
        componentArray[entity].reset();

        // Update systems
        update_entity_in_systems(entity);
    }

    template<typename T>
    T& ECS_Manager::get_component(EntityID entity) {
        std::type_index typeIndex(typeid(T));

        // Ensure component type is registered
        assert(component_type_to_id.find(typeIndex) != component_type_to_id.end() && "Component type not registered.");

        // Ensure entity ID is valid
        assert(entity < entities.size() && "Entity ID out of range.");

        auto& componentArray = get_component_array<T>();

        // Ensure the component exists on the entity
        assert(componentArray[entity] && "Component not found on entity.");

        return *static_cast<T*>(componentArray[entity].get());
    }

    template<typename T>
    bool ECS_Manager::has_component(EntityID entity) const {
        std::type_index typeIndex(typeid(T));

        // Ensure component type is registered
        if (component_type_to_id.find(typeIndex) == component_type_to_id.end()) {
            return false;
        }

        // Ensure entity ID is valid
        assert(entity < entities.size() && "Entity ID out of range.");

        const auto& componentArray = component_arrays.at(typeIndex);
        return componentArray[entity] != nullptr;
    }

    template<typename T>
    std::size_t ECS_Manager::get_component_id() const {
        std::type_index typeIndex(typeid(T));

        // Ensure component type is registered
        assert(component_type_to_id.find(typeIndex) != component_type_to_id.end() && "Component type not registered.");

        return component_type_to_id.at(typeIndex);
    }

    template<typename T>
    std::vector<std::unique_ptr<Component>>& ECS_Manager::get_component_array() {
        std::type_index typeIndex(typeid(T));

        // Ensure component type is registered
        assert(component_arrays.find(typeIndex) != component_arrays.end() && "Component type not registered.");

        return component_arrays[typeIndex];
    }

} // namespace lof

#endif // LOF_ECS_MANAGER_H