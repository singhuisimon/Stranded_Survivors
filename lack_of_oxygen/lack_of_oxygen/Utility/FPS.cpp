/**
 * @file FPS.cpp
 * @brief Implements the FPS class helper functions for calculating current FPS.
 * @author Simon Chan
 * @date September 15, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
// Include header file
#include "FPS.h"

namespace lof {

    FPS::FPS(int window_size)
        : window_size(window_size),
        time_accumulator(0),
        current_fps(0.0f) {
    }

    void FPS::update(int64_t frame_time) {
        // Add the new frame time to the queue and accumulator
        frame_times.push_back(frame_time);
        time_accumulator += frame_time;

        // If we have more frames than the window size, remove the oldest frame
        if (frame_times.size() > static_cast<size_t>(window_size)) {
            time_accumulator -= frame_times.front();
            frame_times.pop_front();
        }

        // Calculate current FPS as the average over the window
        if (time_accumulator > 0) {
            current_fps = (frame_times.size() * 1000000.0f) / time_accumulator; // Since time is in microseconds
        }
        else {
            current_fps = 0.0f;
        }
    }

    float FPS::get_fps() const {
        return current_fps;
    }

    void FPS::reset() {
        frame_times.clear();
        time_accumulator = 0;
        current_fps = 0.0f;
    }

} // namespace lof
