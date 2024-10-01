/**
 * @file FPS_Manager.h
 * @brief Defines the FPS_Manager class for managing the frame rate of the game loop.
 * @author Simon Chan
 * @date September 20, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef LOF_FPS_MANAGER_H
#define LOF_FPS_MANAGER_H

// Macros for accessing manager singleton instances
#define FPSM lof::FPS_Manager::get_instance()

 // Include base Manager class
#include "Manager.h"

// Include necessary headers
#include <cstdint>
#include "../Utility/Clock.h"
#include "../Utility/FPS.h"

namespace lof {

    /**
     * @class FPS_Manager
     * @brief Manages the frame rate of the game loop to maintain consistent FPS.
     *
     * This class is implemented as a singleton and is responsible for
     * controlling the timing of the game loop to achieve a target FPS.
     */
    class FPS_Manager : public Manager {
    private:
        FPS_Manager();                      // Private since a singleton.

        // Constant variables
        const int TARGET_FPS;                       // Target frames per second
        const int64_t MICROSECONDS_PER_SECOND;      // Microseconds in a second
        const int64_t TARGET_TIME;                  // Target frame time in microseconds

        // Mutable variables
        int64_t adjust_time;                        // Adjustment time for sleep discrepancies
        int64_t last_frame_start_time;              // Time when the last frame started

        // Clock for timing
        Clock clock;

        // Delta time for the current frame
        float delta_time;

        // FPS calculator from FPS.cpp
        FPS fps_calculator;

    public:
        /**
         * @brief Get the singleton instance of the FPS_Manager.
         * @return Reference to the FPS_Manager instance.
         */
        static FPS_Manager& get_instance();

        // Delete copy constructor and assignment operator
        FPS_Manager(const FPS_Manager&) = delete;
        FPS_Manager& operator=(const FPS_Manager&) = delete;

        /**
         * @brief Start up the FPS_Manager.
         * @return 0 if successful, else negative number.
         */
        int start_up() override;

        /**
         * @brief Shut down the FPS_Manager.
         */
        void shut_down() override;

        /**
         * @brief Call at the beginning of each frame to start timing.
         */
        void frame_start();

        /**
         * @brief Call at the end of each frame to maintain target FPS.
         */
        void frame_end();

        /**
         * @brief Get the delta time for the current frame.
         * @return Delta time in seconds as float.
         */
        float get_delta_time() const;

        /**
         * @brief Get the current calculated FPS.
         * @return Current FPS as a float.
         */
        float get_current_fps() const;

        /**
         * @brief Reset the FPS calculator.
         */
        void reset_fps();
    };

} // end of namespace lof

#endif // LOF_FPS_MANAGER_H
