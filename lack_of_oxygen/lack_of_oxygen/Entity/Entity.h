/**
 * @file Entity.h
 * @brief Defines the Entity class for the Entity Component System (ECS).
 * @details This file contains the Entity class definition, which represents game objects within the ECS architecture.
 *          It manages component associations for each entity using a bit mask.
 * @author Simon Chan
 * @date September 15, 2024
 */

#ifndef LOF_ENTITY_H
#define LOF_ENTITY_H

// Include standard headers
#include <cstdint>
#include <bitset>

 /**
  * @typedef EntityID
  * @brief Alias for the data type used to represent entity identifiers.
  */
using EntityID = std::uint32_t;

/**
 * @brief Maximum number of different component types allowed in the system.
 */
const std::size_t MAX_COMPONENTS = 32;

/**
 * @typedef ComponentMask
 * @brief Bitset used to efficiently store which components an entity has.
 */
using ComponentMask = std::bitset<MAX_COMPONENTS>;

namespace lof {

    /**
     * @class Entity
     * @brief Represents a game object in the Entity Component System.
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
         * @return EntityID The entity's unique identifier.
         */
        EntityID get_id() const;

        /**
         * @brief Add a component to the entity.
         * @param component_id The ID of the component type to add.
         */
        void add_component(std::size_t component_id);

        /**
         * @brief Remove a component from the entity.
         * @param component_id The ID of the component type to remove.
         */
        void remove_component(std::size_t component_id);

        /**
         * @brief Check if the entity has a specific component.
         * @param component_id The ID of the component type to check for.
         * @return bool True if the entity has the component, false otherwise.
         */
        bool has_component(std::size_t component_id) const;

        /**
         * @brief Get the component mask of the entity.
         * @return ComponentMask The entity's component mask.
         */
        ComponentMask get_component_mask() const;
    };

} // namespace lof

#endif // LOF_ENTITY_H