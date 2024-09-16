/**
 * @file ECS_Manager.h
 * @brief Defines the ECS_Manager class for managing the Entity Component System (ECS).
 * @author Simon Chan
 * @date September 15, 2024
 */

#pragma once

#ifndef ECS_MANAGER_H
#define ECS_MANAGER_H

 // Include base Manager class
#include "Manager.h" 

 // Include ECS headers
#include "../Entity/Entity.h"
#include "../Component/Component.h"
#include "../System/System.h"
#include "../System/Movement_System.h"

 // Include Log_Manager for logging
#include "Log_Manager.h"

 // Include standard headers
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <mutex>

namespace lof {

    class System; // Forward declaration

    #define ECSM lof::ECS_Manager::get_instance()

    class ECS_Manager : public Manager {
    private:
        static std::unique_ptr<ECS_Manager> instance;
        static std::once_flag once_flag;

        std::vector<std::unique_ptr<Entity>> entities;
        std::unordered_map<std::type_index, ComponentID> component_type_to_id;
        std::unordered_map<ComponentID, std::type_index> id_to_component_type;
        ComponentID next_component_id = 0;

        std::unordered_map<ComponentID, std::unordered_map<EntityID, std::shared_ptr<Component>>> component_arrays;

        std::vector<std::unique_ptr<System>> systems;

        /**
         * @brief Private constructor to enforce singleton pattern.
         */
        ECS_Manager();

    public:
        ECS_Manager(const ECS_Manager&) = delete;
        ECS_Manager& operator=(const ECS_Manager&) = delete;

        /**
         * @brief Virtual destructor for ECS_Manager.
         */
        virtual ~ECS_Manager();

        /**
         * @brief Gets the singleton instance of ECS_Manager.
         * @return Reference to the ECS_Manager instance.
         */
        static ECS_Manager& get_instance();

        /**
         * @brief Starts up the ECS_Manager.
         * @return 0 if successful, else a negative number.
         */
        int start_up() override;

        /**
         * @brief Shuts down the ECS_Manager.
         */
        void shut_down() override;

        /**
         * @brief Creates a new entity.
         * @return The ID of the newly created entity.
         */
        EntityID create_entity();

        /**
         * @brief Adds a system to the ECS.
         * @param system A unique pointer to the system to be added.
         */
        void add_system(std::unique_ptr<System> system);

        /**
         * @brief Updates all systems.
         * @param delta_time The time elapsed since the last update.
         */
        void update(float delta_time);

        /**
         * @brief Gets all entities in the ECS.
         * @return A const reference to the vector of entities.
         */
        const std::vector<std::unique_ptr<Entity>>& get_entities() const;

        /**
         * @brief Registers a new component type.
         * @tparam T The type of the component to register.
         * @return The ID assigned to the registered component type.
         */
        template<typename T> ComponentID register_component();

        /**
         * @brief Gets the ID of a registered component type.
         * @tparam T The type of the component.
         * @return The ID of the component type.
         */
        template<typename T> ComponentID get_component_id();

        /**
         * @brief Adds a component to an entity.
         * @tparam T The type of the component to add.
         * @param entity The ID of the entity to add the component to.
         * @param component The component to add.
         */
        template<typename T> void add_component(EntityID entity, T component);

        /**
         * @brief Removes a component from an entity.
         * @tparam T The type of the component to remove.
         * @param entity The ID of the entity to remove the component from.
         */
        template<typename T> void remove_component(EntityID entity);

        /**
         * @brief Gets a component from an entity.
         * @tparam T The type of the component to get.
         * @param entity The ID of the entity to get the component from.
         * @return A reference to the requested component.
         */
        template<typename T> T& get_component(EntityID entity);
    };

    // Template method implementations
    template<typename T>
    ComponentID ECS_Manager::register_component() {
        const std::type_index type_name = typeid(T);
        if (component_type_to_id.find(type_name) != component_type_to_id.end()) {
            return component_type_to_id[type_name];
        }

        ComponentID id = next_component_id++;
        component_type_to_id.emplace(type_name, id);
        id_to_component_type.emplace(id, type_name);

        component_arrays.emplace(id, std::unordered_map<EntityID, std::shared_ptr<Component>>());

        return id;
    }

    template<typename T>
    ComponentID ECS_Manager::get_component_id() {
        const std::type_index type_name = typeid(T);
        auto it = component_type_to_id.find(type_name);
        if (it == component_type_to_id.end()) {
            std::string error_msg = "Component not registered: " + std::string(type_name.name());
            LM.write_log("ECS_Manager Error: %s", error_msg.c_str());
            throw std::runtime_error(error_msg);
        }
        return it->second;
    }

    template<typename T>
    void ECS_Manager::add_component(EntityID entity, T component) {
        try {
            ComponentID component_id = get_component_id<T>();
            auto& component_map = component_arrays[component_id];
            component_map.emplace(entity, std::make_shared<T>(std::move(component)));
            entities[entity]->add_component(component_id);
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager Error: Failed to add component to entity %u. %s", entity, e.what());
            throw;
        }
    }

    template<typename T>
    void ECS_Manager::remove_component(EntityID entity) {
        try {
            ComponentID component_id = get_component_id<T>();
            auto& component_map = component_arrays[component_id];
            if (component_map.erase(entity) == 0) {
                std::string error_msg = "Entity " + std::to_string(entity) + " does not have the component to remove";
                LM.write_log("ECS_Manager Error: %s", error_msg.c_str());
                throw std::runtime_error(error_msg);
            }
            entities[entity]->remove_component(component_id);
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager Error: Failed to remove component from entity %u. %s", entity, e.what());
            throw;
        }
    }

    template<typename T>
    T& ECS_Manager::get_component(EntityID entity) {
        try {
            ComponentID component_id = get_component_id<T>();
            auto& component_map = component_arrays[component_id];
            auto it = component_map.find(entity);
            if (it == component_map.end()) {
                std::string error_msg = "Entity " + std::to_string(entity) + " does not have the requested component";
                LM.write_log("ECS_Manager Error: %s", error_msg.c_str());
                throw std::runtime_error(error_msg);
            }
            return *std::static_pointer_cast<T>(it->second);
        }
        catch (const std::exception& e) {
            LM.write_log("ECS_Manager Error: Failed to get component for entity %u. %s", entity, e.what());
            throw;
        }
    }

} // namespace lof

#endif // ECS_MANAGER_H