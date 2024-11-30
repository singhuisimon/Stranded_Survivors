/**
 * @file Clock.cpp
 * @brief Implements the Clock class methods for precise time measurements and sleep functionality.
 * @author Simon Chan (100%)
 * @date September 15, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
// Include header file
#include "Clock.h"

// Include standard headers
#include <chrono>
#include <thread>

namespace lof {

    Clock::Clock() {
        int64_t current_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();

        m_previous_time = current_time;
        m_start_time = current_time; // Initialize start time
    }

    int64_t Clock::delta() {
        int64_t current_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();

        int64_t delta_time = current_time - m_previous_time;
        m_previous_time = current_time;

        return delta_time;
    }

    int64_t Clock::split() const {
        int64_t current_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();

        return current_time - m_previous_time;
    }

    int64_t Clock::split_total() const {
        int64_t current_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();

        return current_time - m_start_time;
    }

    void Clock::sleep(int64_t microseconds) {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
  }

} // end of namespace lof