/**
 * @file System.h
 * @brief Defines the base System class for the Entity Component System (ECS).
 * @details Systems are responsible for implementing game logic that operates on entities with specific components.
 * @author Simon Chan
 * @date September 15, 2024
 */

#ifndef LOF_SYSTEM_H
#define LOF_SYSTEM_H

// Include standard headers
#include <string>

namespace lof {

    /**
     * @class System
     * @brief Abstract base class for all systems in the ECS.
     */
    class System {
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
    };

} // namespace lof

#endif // LOF_SYSTEM_H