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

        // Helper member functions
        template<typename T>
        std::vector<std::unique_ptr<Component>>& get_component_array();

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

        // Entity management
        EntityID create_entity();

        /**
         * @brief Clone an entity based on a prefab.
         * @param prefab_name The name of the prefab to clone from.
         * @return The ID of the newly created entity, or INVALID_ENTITY_ID if failed.
         */
        EntityID clone_entity_from_prefab(const std::string& prefab_name);

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

        // Accsessing each system
        const std::vector<std::unique_ptr<System>>& get_systems() const;

        // Access entities
        const std::vector<std::unique_ptr<Entity>>& get_entities() const;
    };

} // namespace lof

// Include template implementations at the end of the header file
#include "ECS_Manager.tpp"

#endif // LOF_ECS_MANAGER_H
