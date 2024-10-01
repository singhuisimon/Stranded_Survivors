/**
 * @file FPS_Manager.cpp
 * @brief Implements the FPS_Manager class methods for managing frame rate control in the game loop.
 * @date September 20, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
 // Include header file
#include "FPS_Manager.h"

// Include other managers
#include "Log_Manager.h"

// Include utility function
#include "../Utility/Constant.h"

// Include standard headers
#include <iostream>

namespace lof {

    FPS_Manager::FPS_Manager()
        : TARGET_FPS(DEFAULT_TARGET_FPS),
        MICROSECONDS_PER_SECOND(DEFAULT_MICROSECONDS_PER_SECOND),
        TARGET_TIME(DEFAULT_TARGET_TIME),
        adjust_time(DEFAULT_ADJUST_TIME),
        delta_time(DEFAULT_DELTA_TIME),
        last_frame_start_time(DEFAULT_LAST_FRAME_START_TIME),
        fps_calculator(DEFAULT_TARGET_FPS) { // Initialize with TARGET_FPS
        set_type("FPS_Manager");
        LM.write_log("FPS_Manager::FPS_Manager(): Initialized with TARGET_FPS = %d.", TARGET_FPS);
    }

    FPS_Manager& FPS_Manager::get_instance() {
        static FPS_Manager instance;
        return instance;
    }

    int FPS_Manager::start_up() {
        if (is_started()) {
            LM.write_log("FPS_Manager::start_up(): Already started.");
            return 0; // Already started
        }

        // Initialize timing variables
        adjust_time = DEFAULT_ADJUST_TIME;
        clock.delta(); // Initialize clock

        m_is_started = true;
        LM.write_log("FPS_Manager::start_up(): FPS_Manager started successfully.");
        return 0;
    }

    void FPS_Manager::shut_down() {
        if (!is_started()) {
            LM.write_log("FPS_Manager::shut_down(): FPS_Manager is not started.");
            return;
        }

        // Reset variables
        adjust_time = DEFAULT_ADJUST_TIME;
        delta_time = DEFAULT_DELTA_TIME;
        last_frame_start_time = DEFAULT_LAST_FRAME_START_TIME;
        fps_calculator.reset();

        m_is_started = false;
        LM.write_log("FPS_Manager::shut_down(): FPS_Manager shut down successfully.");
    }

    void FPS_Manager::frame_start() {
        // Get the current time
        int64_t current_time = clock.split_total();

        // Calculate delta_time as the time since the last frame
        if (last_frame_start_time != DEFAULT_LAST_FRAME_START_TIME) {
            int64_t frame_duration = current_time - last_frame_start_time;
            delta_time = static_cast<float>(frame_duration) / MICROSECONDS_PER_SECOND;
            LM.write_log("FPS_Manager::frame_start(): Calculated delta_time = %.6f seconds.", delta_time);
        }

        // Update last_frame_start_time
        last_frame_start_time = current_time;

        // Start of frame timing
        clock.delta();
        LM.write_log("FPS_Manager::frame_start(): Frame timing started.");
    }

    void FPS_Manager::frame_end() {
        // Measure the time taken for the main loop to complete so far
        int64_t loop_time = clock.split();

        // Calculate how long the program should sleep to maintain the target frame time
        int64_t intended_sleep_time = TARGET_TIME - loop_time - adjust_time;

        // If the calculated sleep time is positive, the program will sleep for that duration
        if (intended_sleep_time > 0) {
            Clock::sleep(intended_sleep_time);
            LM.write_log("FPS_Manager::frame_end(): Slept for %lld microseconds to maintain target FPS.", intended_sleep_time);
        }
        else {
            LM.write_log("FPS_Manager::frame_end(): No sleep needed. Loop time exceeded target.");
        }

        // Measure the actual sleep time by splitting the clock again after sleep
        int64_t total_time = clock.split(); // Total time since frame_start()
        int64_t actual_sleep_time = total_time - loop_time;

        // Adjust the sleep time for the next frame, compensating for any discrepancy
        adjust_time = actual_sleep_time - intended_sleep_time;
        LM.write_log("FPS_Manager::frame_end(): Adjust time set to %lld microseconds.", adjust_time);

        // Update FPS calculator with the total time for the frame (in microseconds)
        fps_calculator.update(static_cast<int64_t>(delta_time * MICROSECONDS_PER_SECOND));

        // Log current FPS
        LM.write_log("FPS_Manager::frame_end(): Current FPS = %.2f.", fps_calculator.get_fps());
    }

    float FPS_Manager::get_delta_time() const {
        LM.write_log("FPS_Manager::get_delta_time(): Returning delta_time = %.6f seconds.", delta_time);
        return delta_time;
    }

    float FPS_Manager::get_current_fps() const {
        float current_fps = fps_calculator.get_fps();
        LM.write_log("FPS_Manager::get_current_fps(): Current FPS = %.2f.", current_fps);
        return current_fps;
    }

    void FPS_Manager::reset_fps() {
        fps_calculator.reset();
        LM.write_log("FPS_Manager::reset_fps(): FPS calculator reset.");
    }

} // namespace lof
