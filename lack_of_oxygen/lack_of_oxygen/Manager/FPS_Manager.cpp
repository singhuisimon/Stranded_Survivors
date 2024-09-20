/**
 * @file FPS_Manager.cpp
 * @brief Implements the FPS_Manager class methods for managing frame rate control in the game loop.
 * @author Simon Chan
 * @date September 20, 2024
 */

#include "FPS_Manager.h"

 // Include other managers
#include "Log_Manager.h"

// Include standard headers
#include <iostream>

namespace lof {

    FPS_Manager::FPS_Manager()
        : TARGET_FPS(60),
        MICROSECONDS_PER_SECOND(1000000),
        TARGET_TIME(MICROSECONDS_PER_SECOND / TARGET_FPS),
        adjust_time(0),
        delta_time(0.0f),
        last_frame_start_time(0),
        fps_calculator(60) {
        set_type("FPS_Manager");
    }

    FPS_Manager& FPS_Manager::get_instance() {
        static FPS_Manager instance;
        return instance;
    }

    int FPS_Manager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        // Initialize timing variables
        adjust_time = 0;
        clock.delta(); // Initialize clock

        m_is_started = true;
        return 0;
    }

    void FPS_Manager::shut_down() {
        if (!is_started()) {
            return;
        }

        // Reset variables
        m_is_started = false;
    }

    void FPS_Manager::frame_start() {
        // Get the current time
        int64_t current_time = clock.split_total();

        // Calculate delta_time as the time since the last frame
        if (last_frame_start_time != 0) {
            int64_t frame_duration = current_time - last_frame_start_time;
            delta_time = static_cast<float>(frame_duration) / MICROSECONDS_PER_SECOND;
        }

        // Update last_frame_start_time
        last_frame_start_time = current_time;

        // Start of frame timing
        clock.delta();
    }

    void FPS_Manager::frame_end() {
        // Measure the time taken for the main loop to complete so far
        int64_t loop_time = clock.split();

        // Calculate how long the program should sleep to maintain the target frame time.
        int64_t intended_sleep_time = TARGET_TIME - loop_time - adjust_time;

        // If the calculated sleep time is positive, the program will sleep for that duration
        if (intended_sleep_time > 0) {
            Clock::sleep(intended_sleep_time);
        }

        // Measure the actual sleep time by splitting the clock again after sleep
        int64_t total_time = clock.split(); // Total time since frame_start()
        int64_t actual_sleep_time = total_time - loop_time;

        // Adjust the sleep time for the next frame, compensating for any discrepancy
        adjust_time = actual_sleep_time - intended_sleep_time;

        // Update FPS calculator with the total time for the frame (in microseconds)
        fps_calculator.update(static_cast<int64_t>(delta_time * MICROSECONDS_PER_SECOND));

        // Debug output
        std::cout << "Loop Time: " << loop_time
            << ", Intended Sleep: " << intended_sleep_time
            << ", Actual Sleep: " << actual_sleep_time
            << ", Adjust Time: " << adjust_time << std::endl;
    }


    float FPS_Manager::get_delta_time() const {
        return delta_time;
    }

    float FPS_Manager::get_current_fps() const {
		return fps_calculator.get_fps();
	}

    void FPS_Manager::reset_fps() {
		fps_calculator.reset();
	}

} // end of namespace lof
