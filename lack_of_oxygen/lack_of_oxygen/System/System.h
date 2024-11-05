/**
 * @file System.h
 * @brief Defines the base System class for the Entity Component System (ECS).
 * @author Simon Chan(63.636%), Liliana (36.363%)
 * @date September 15, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_SYSTEM_H
#define LOF_SYSTEM_H

// Include standard headers
#include <string>
#include <unordered_set>       // For std::unordered_set

// Include other necessary headers
#include "../Utility/Constant.h"
#include "../Utility/Type.h"  // Include shared types

namespace lof {

    /**
     * @class System
     * @brief Abstract base class for all systems in the ECS.
     */
    class System { 
    protected:
        Signature signature;                      ///< Components required by the system
        std::unordered_set<EntityID> entities;    ///< Entities matching the system's signature

    private:

        int64_t system_time = DEFAULT_START_TIME; // Private data member to store system's consumption time in game loop.

    public:
        /**
         * @brief Pure virtual function to update the system.
         * @param delta_time The time elapsed since the last update, typically in seconds.
         */
        virtual void update(float delta_time) = 0;

        /**
         * @brief Pure virtual function to get the system's type as a string.
         * @return The system's type name.
         */
        virtual std::string get_type() const = 0;

        /**
         * @brief Virtual destructor for the System class.
         */
        virtual ~System() = default;

        /**
         * @brief Virtual function to get the system time private member of the System class.
         * @return The system's consumption time.
         */
        int64_t get_time() const {
            return system_time;
        }

        /**
         * @brief Virtual function to set the system time private member of the System class.
         * @param time The value of time to set the private member variable.
         */
        void set_time(int64_t time) {
            system_time = time;
        }

        /**
         * @brief Add an entity to the system's entity list.
         * @param entity The ID of the entity to add.
         */
        void add_entity(EntityID entity) {
            entities.insert(entity);
        }

        /**
         * @brief Remove an entity from the system's entity list.
         * @param entity The ID of the entity to remove.
         */
        void remove_entity(EntityID entity) {
            entities.erase(entity);
        }

        /**
         * @brief Get the system's component signature.
         * @return The system's signature.
         */
        const Signature& get_signature() const {
            return signature;
        }

        /**
         * @brief Get the list of entities the system processes.
         * @return A constant reference to the set of entity IDs.
         */
        const std::unordered_set<EntityID>& get_entities() const {
            return entities;
        }

        // Add this to System.h in the System class:
        bool has_entity(EntityID entity) const {
            return entities.find(entity) != entities.end();
        }
    };
} // namespace lof

#endif // LOF_SYSTEM_H