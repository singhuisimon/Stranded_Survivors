/**
 * @file FPS.h
 * @brief Defines the FPS class for calculating and retrieving the current FPS.
 * @author Simon Chan
 * @date September 15, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#ifndef LOF_FPS_H
#define LOF_FPS_H

 // Include necessary headers
#include <deque>
#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>

namespace lof {

    /**
     * @class FPS
     * @brief Calculates and provides access to the current FPS (frames per second).
     *
     * This class keeps track of frame times and calculates the average FPS over a specified interval.
     */
    class FPS {
    private:
        int window_size;                         ///< Number of frames to average over.
        std::deque<int64_t> frame_times;         ///< Queue storing frame times in microseconds.
        int64_t time_accumulator;                ///< Accumulated time of frames in the window.
        float current_fps;                       ///< Calculated current FPS.

    public:
        /**
         * @brief Constructor for the FPS class.
         * @param window_size Number of frames to calculate the average FPS over.
         */
        FPS(int window_size = 60);

        /**
         * @brief Call this method at the end of each frame to update FPS calculation.
         * @param frame_time Time taken for the current frame in microseconds.
         */
        void update(int64_t frame_time);

        /**
         * @brief Get the current calculated FPS.
         * @return Current FPS as a float.
         */
        float get_fps() const;

        /**
         * @brief Reset the FPS calculator.
         */
        void reset();
    };

    /**
    * @brief Calculates and prints out the percentage of the given managerm's time with that of the entire game loop.
    * @param gm_time Time consumption of the game loop (Game Manager's consumption time).
    * @param system_time Time consumption of the manager/system.
    * @param name Name of manager/system.
    */
    void system_performance(int64_t gm_time, int64_t system_time, std::string name);

} // namespace lof

#endif // LOF_FPS_H
