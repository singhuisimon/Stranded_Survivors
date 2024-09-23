/**
 * @file ECS_Manager.h
 * @brief Declares the ECS_Manager class for managing the ECS.
 * @date September 21, 2024
 */

#ifndef LOF_ECS_MANAGER_H
#define LOF_ECS_MANAGER_H

// Macros for accessing manager singleton instances
#define ECSM lof::ECS_Manager::get_instance()

 // Include base headers
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

        // Member variables
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<System>> systems;

        // Component storage
        std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> component_arrays;
        std::unordered_map<std::type_index, std::size_t> component_type_to_id;
        std::unordered_map<std::size_t, const std::type_info*> id_to_component_type;
        std::size_t next_component_id = 0;

        // Helper methods
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

        // Component management
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

        // Access entities
        const std::vector<std::unique_ptr<Entity>>& get_entities() const;

        // Load entities from configuration data
        int load_entities(const std::vector<EntityConfig>& entities_config,
            const std::unordered_map<std::string, std::shared_ptr<Model>>& models);
    };

} // namespace lof

// Include template implementations at the end of the header file
#include "ECS_Manager.tpp"

#endif // LOF_ECS_MANAGER_H
