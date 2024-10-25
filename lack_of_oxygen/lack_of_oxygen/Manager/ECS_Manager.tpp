/**
 * @file ECS_Manager.tpp
 * @brief Definition for the ECS_Manager template class for managing the ECS.
 * @author Simon Chan (100%)
 * @date September 21, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

namespace lof {

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
    bool ECS_Manager::has_component(EntityID entity) const {
        std::type_index typeIndex(typeid(T));

        // Ensure component type is registered
        assert(component_type_to_id.find(typeIndex) != component_type_to_id.end() && "Component type not registered.");

        // Ensure entity ID is valid
        assert(entity < entities.size() && "Entity ID out of range.");

        return entities[entity]->has_component(component_type_to_id.at(typeIndex));
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

