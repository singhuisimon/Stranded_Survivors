/**
 * @file Entity.h
 * @brief Defines the Entity class for the Entity Component System (ECS).
 * @author Simon Chan (100%)
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_ENTITY_H
#define LOF_ENTITY_H

// Include standard headers
#include <cstdint>
#include <bitset>

 /// Maximum number of components an entity can have
const std::size_t MAX_COMPONENTS = 32;

/// Alias for the entity identifier type
using EntityID = std::uint32_t;

/// Alias for the data structure that is used to store component
using ComponentMask = std::bitset<MAX_COMPONENTS>;

/// Constant representing an invalid entity ID
constexpr EntityID INVALID_ENTITY_ID = std::numeric_limits<EntityID>::max();

namespace lof {

    /**
     * @brief Represents a game object in the Entity Component System.
     * @details An Entity is essentially just an ID and a mask of components. It doesn't
     *          store component data directly, but rather acts as a handle to access
     *          components stored elsewhere in the ECS.
     */
    class Entity {
    private:
        EntityID id;           ///< Unique identifier for the entity
        ComponentMask mask;    ///< Bitset indicating which components the entity has

    public:
        /**
         * @brief Constructor for Entity.
         * @param id Unique identifier for the new entity.
         */
        Entity(EntityID id);

        /**
         * @brief Get the unique identifier of the entity.
         * @return The entity's unique identifier.
         */
        EntityID get_id() const;

        /**
         * @brief Add a component to the entity.
         * @details Sets the bit corresponding to the component_id in the mask.
         * @param component_id The ID of the component type to add.
         */
        void add_component(std::size_t component_id);

        /**
         * @brief Remove a component from the entity.
         * @details Resets the bit corresponding to the component_id in the mask.
         * @param component_id The ID of the component type to remove.
         */
        void remove_component(std::size_t component_id);

        /**
         * @brief Check if the entity has a specific component.
         * @details Tests the bit corresponding to the component_id in the mask.
         * @param component_id The ID of the component type to check for.
         * @return True if the entity has the component, false otherwise.
         */
        bool has_component(std::size_t component_id) const;

        /**
         * @brief Get the component mask of the entity.
         * @details Returns the entire component mask, which can be useful for
         *          system queries in the ECS.
         * @return The entity's component mask.
         */
        ComponentMask get_component_mask() const;
    };

} // namespace lof

#endif // LOF_ENTITY_H