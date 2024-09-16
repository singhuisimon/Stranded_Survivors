/**
 * @file Movement_System.h
 * @brief Defines the Movement_System class for the ECS.
 * @details Updates entity positions based on their velocities each frame.
 * @author Simon Chan
 * @date September 15, 2024
 */

#ifndef LOF_MOVEMENT_SYSTEM_H
#define LOF_MOVEMENT_SYSTEM_H

// Include file dependencies
#include "System.h"

 // Forward declaration
namespace lof {
    class ECS_Manager;
}

namespace lof {

    /**
     * @class Movement_System
     * @brief System responsible for updating entity positions based on velocity.
     */
    class Movement_System : public System {
    private:
        ECS_Manager& ecs_manager; ///< Reference to the ECS Manager

    public:
        /**
         * @brief Constructor for Movement_System.
         * @param manager Reference to the ECS_Manager.
         */
        Movement_System(ECS_Manager& manager);

        /**
         * @brief Updates the system.
         * @param delta_time The time elapsed since the last update, in seconds.
         */
        void update(float delta_time) override;
    };

} // namespace lof

#endif // LOF_MOVEMENT_SYSTEM_H