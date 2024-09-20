/**
 * @file FPS.h
 * @brief Defines the FPS class for calculating and retrieving the current FPS.
 */

#ifndef LOF_FPS_H
#define LOF_FPS_H

 // Include necessary headers
#include <deque>
#include <cstdint>

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

} // namespace lof

#endif // LOF_FPS_H
