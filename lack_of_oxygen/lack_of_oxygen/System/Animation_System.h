/**
 * @file Animation_System.h
 * @brief Defines the Animation_System class for the ECS.
 * @details Updates animation of the entity based on the animation and frame data.
 * @author Chua Wen Bin Kenny (100%)
 * @date October 25, 2024
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#ifndef LOF_ANIMATION_SYSTEM_H
#define LOF_ANIMATION_SYSTEM_H

 // Include file dependencies
#include "System.h"
#include "../Manager/ECS_Manager.h"
#include "../Component/Component.h"
#include "../Manager/Graphics_Manager.h" // To access animation storage

// Include Utility headers
#include "../Utility/constant.h"         // To access constants 
#include "../Utility/Clock.h"            // To monitor time delay
    
namespace lof {

    /**
     * @class Animation_System
     * @brief System responsible for updating animation and frames sequence.
     */
    class Animation_System : public System {

        // Player action flag
        enum Player_Action {
            IDLE,
            MOVING,
            MINING,
            MOVING_N_MINING
        };
        int player_action = IDLE;
        bool has_mined = false;

    public:
        /**
         * @brief Constructor for Animation_System.
         * @param manager Reference to the ECS_Manager.
         */
        Animation_System();

        /**
         * @brief Returns the system's type.
         * @return The string "Animation_System".
         */
        std::string get_type() const override;

        /**
         * @brief Updates the animation data of the entity based on the frame and time.
         * @param delta_time The time elapsed since the last update, typically in seconds.
         */
        void update(float delta_time) override;

    };

} // namespace lof

#endif // LOF_ANIMATION_SYSTEM_H
