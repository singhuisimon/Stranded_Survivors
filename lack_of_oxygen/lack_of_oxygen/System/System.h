/**
 * @file System.h
 * @brief Defines the base System class for the Entity Component System (ECS).
 * @details Systems are responsible for implementing game logic that operates on entities with specific components.
 * @date September 15, 2024
 */

#pragma once

#include <string>

namespace lof {

    /**
     * @class System
     * @brief Abstract base class for all systems in the ECS.
     */
    class System {
    private:

        int64_t system_time = 0; // Private data member to store system's consumption time in game loop.

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
        virtual int64_t get_time() const {
            return system_time;
        }

        /**
         * @brief Virtual function to set the system time private member of the System class.
         * @param time The value of time to set the private member variable.
         */
        virtual void set_time(int64_t time) {
            system_time = time;
        }
    };

} // namespace lof
